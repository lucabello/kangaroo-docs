//
// Created by lucab on 06/05/2019.
//

#include <string>
#include "SharedEditor.h"

SharedEditor::SharedEditor(NetworkServer &s) : _server(s), _counter(0),
    _siteId(0){};

void SharedEditor::localInsert(int index, char value) {
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    prev = (index-1 >= 0 && index-1 < _symbols.size())?
            _symbols.at(index-1).getPosition() : prev;
    succ = (index >= 0 && index < _symbols.size())?
            _symbols.at(index).getPosition() : succ;
    Symbol s {value, _siteId, _counter, _lseq.alloc(prev, succ)};
    _counter++;
    _symbols.insert(_symbols.begin()+index, s);
    Message m {MessageType::INSERT, s}; //prepare and send message
    _server.send(m);
}

void SharedEditor::localErase(int index) {
    if(index < 0 || index >= _symbols.size())
        return;
    Symbol s = _symbols.at(index);
    _symbols.erase(_symbols.begin()+index);
    Message m {MessageType::ERASE, s};
    _server.send(m);
}

void SharedEditor::process(const Message &m) {
    int l=0, r=_symbols.size()-1, middle=-1;
    if(m.getType() == MessageType::INSERT){
        /*
         * After this binary search, the variable middle can be:
         * - -1 if the vector is empty
         * - index of the first element bigger than s
         * - index of the first element smaller than s
         * If the symbol is found, the function will return.
         */
        while(l<=r){
            middle = (l+r)/2;
            if(_symbols.at(middle) < m.getSymbol())
                l = middle+1;
            else if(m.getSymbol() < _symbols.at(middle))
                r = middle-1;
            else
                break;
        }
        if(l<=r) //Symbol found (already inserted)
            return;
        if(middle == -1) //empty vector, never entered while cycle
            _symbols.insert(_symbols.begin(), m.getSymbol());
        else if (m.getSymbol() < _symbols.at(middle))
            _symbols.insert(_symbols.begin()+middle, m.getSymbol());
        else
            _symbols.insert(_symbols.begin()+middle+1, m.getSymbol());
    } else if (m.getType() == MessageType::ERASE) {
        //middle variable is modified or the function returns
        while(l<=r){
            middle = (l+r)/2;
            if(_symbols.at(middle) < m.getSymbol())
                l = middle+1;
            else if(m.getSymbol() < _symbols.at(middle))
                r = middle-1;
            else
                break;
        }
        if(l > r) //Symbol not found (already deleted or empty vector)
            return;
        _symbols.erase(_symbols.begin()+middle);
    }
}

std::string SharedEditor::to_string() {
    std::string result;
    auto it = _symbols.begin();
    for( ; it != _symbols.end() ; ++it)
        result.push_back((*it).getValue());
    return result;
}

void SharedEditor::setSiteId(int id) {
    _siteId = id;
}