#include "SharedEditor.h"
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

SharedEditor::SharedEditor(QWidget *parent, int siteId) : QTextEdit(parent), _lseq(LSEQAllocator(siteId)){}

//SharedEditor::SharedEditor(const QString &text, QWidget *parent, int siteId) : QTextEdit(text, parent), _lseq(LSEQAllocator(siteId)){}

SharedEditor::~SharedEditor(){}

bool isKeyPrintable(QKeyEvent * e){
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        return true;
    int category = e->text().unicode()[0].category();
    qDebug() << "Category = " << category;
    if(category >= 14 && category <= 29)
        return true;
    else if(category >= 0 && category <= 8)
        return true;
    else
        return false;
}

bool isKeyPaste(QKeyEvent * e){
    //paste is in QChar category 9! change this function and test it
    int k = e->key();
    int modifiers = e->modifiers();
    if(modifiers == Qt::ControlModifier && k == Qt::Key_V)
        return true;
    return false;
}

void SharedEditor::keyPressEvent(QKeyEvent * e){
    if(e->key() == Qt::Key_0){
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << "Debug key pressed.";
        qDebug() << "Hope for the best. Good luck.";
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << to_string();
        qDebug() << "++++++++++++++++++++++++++++";
        return;
    }
    if(e->key() == Qt::Key_9){
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << "Connecting to server...";
        qDebug() << "++++++++++++++++++++++++++++";
        emit connectToServer();
        return;
    }
    if(e->key() == Qt::Key_8){
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << "Tesing serialization...";
        qDebug() << "++++++++++++++++++++++++++++";
        for(Symbol s : _symbols){
            qDebug() << "SER  " << QString::fromStdString(s.toString());
            qDebug() << "UNSER" << QString::fromStdString(Symbol::unserialize(Symbol::serialize(s)).toString());
            qDebug() << "---";
        }
        qDebug() << "++++++++++++++++++++++++++++";
        return;
    }
    qDebug() << "Keypress caught!";
    qDebug() << "e->key() = " << e->key();
    if(isKeyPrintable(e)){
        qDebug() << "- Printable key: " << e->text();
        eraseSelectedText(e);
        //insert new character
        wchar_t c[1];
        e->text().toWCharArray(c);
        localInsert(this->textCursor().position(), c[0]);
    } else if(e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete){
        eraseSelectedText(e);
    }
    else if(isKeyPaste(e)){
        //qDebug() << "- You pasted: " << QApplication::clipboard()->mimeData()->html();
        return;
    }
    QTextEdit::keyPressEvent(e);
}
















int SharedEditor::editorToVectorIndex(int i){
    int a=0, count=0;
    for(; a <= i+count && a < _symbols.size() && !_symbols.empty(); a++){
        if(_symbols.at(a).isStyle()){
            count++;
        }
    }
    return count+i;
}

int SharedEditor::vectorToEditorIndex(int i){
    int a=0, count=0;
    for(; a < i && a < _symbols.size() && !_symbols.empty(); a++){
        if(_symbols.at(a).isStyle()){
            count++;
        }
    }
    return i-count;
}

void SharedEditor::localInsert(int index, wchar_t value) {
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    index = editorToVectorIndex(index); //sum number of tag symbols to index
    prev = (index-1 >= 0 && index-1 < _symbols.size())?
            _symbols.at(index-1).getPosition() : prev;
    succ = (index >= 0 && index < _symbols.size())?
            _symbols.at(index).getPosition() : succ;
    Symbol s {value, _siteId, _counter, _lseq.alloc(prev, succ)};
    _counter++;
    _symbols.insert(_symbols.begin()+index, s);
    if(value == '\r'){
        localInsertStyle(index+1, Symbol(StyleType::Paragraph, AlignmentType::AlignLeft, -1, -1, std::vector<int>()));
    }
    Message m {MessageType::Insert, s}; //prepare and send message
    //qDebug() << QString::fromStdString(m.toString());
    //EMISSION OF SIGNAL SHOULD HAPPEN IN MESSAGE QUEUE, BY ANOTHER THREAD
    emit packetReady(m); //DEBUG TESTING, NEEDS TO BE ADDED TO STYLE TOO
    //_mqOut.push(m);
}

void SharedEditor::localInsertStyle(int index, Symbol styleSymbol){
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    prev = (index-1 >= 0 && index-1 < _symbols.size())?
            _symbols.at(index-1).getPosition() : prev;
    succ = (index >= 0 && index < _symbols.size())?
            _symbols.at(index).getPosition() : succ;
    Symbol s {styleSymbol, _siteId, _counter, _lseq.alloc(prev, succ)};
    _counter++;
    _symbols.insert(_symbols.begin()+index, s);
    Message m {MessageType::Insert, s}; //prepare and send message
    //EMISSION OF SIGNAL SHOULD HAPPEN IN MESSAGE QUEUE, BY ANOTHER THREAD
    emit packetReady(m); //DEBUG TESTING, NEEDS TO BE ADDED TO STYLE TOO
    //_mqOut.push(m);
}

void SharedEditor::localSetSimpleStyle(int start, int end, Symbol s){
    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end);
    bool insertOpenTag = true, insertCloseTag = true;
    int i;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        if(_symbols.at(i).isOpeningOf(s)){
            //if(i == start-1) //continuing previous tag
            //    insertOpenTag = false;
            break;
        }
        if(_symbols.at(i).isSameStyleAs(s)){
            insertOpenTag = false;
            break;
        }

    }
    for(i=end; i<_symbols.size(); i++){
        if(_symbols.at(i).isSameStyleAs(s)){
            break;
        }
        if(_symbols.at(i).isOpeningOf(s)){
            insertCloseTag = false;
            break;
        }
    }
    if(!insertOpenTag && !insertCloseTag)
        return;
    //remove all -style- tags between end and start
    //start from last so we don't ruin indexes
    if(end >= _symbols.size()){
        localInsertStyle(end, Symbol::getClosedStyle(s));
        insertCloseTag = false;
        end--;
    }
    for(i=end; i>=start; i--){
        if(_symbols.at(i).isStyle() && Symbol::areSimilarTags(_symbols.at(i), s)){
            localErase(i);
            end--;
        }
    }
    if(insertCloseTag)
        localInsertStyle(end, Symbol::getClosedStyle(s));
    if(insertOpenTag)
        localInsertStyle(start, s);
}

void SharedEditor::localUnsetStyle(int start, int end, Symbol s){
    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end)-1;
    bool insertOpenTag = false, insertCloseTag = false;
    int i;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        if(_symbols.at(i).isOpeningOf(s)){
            break;
        }
        if(_symbols.at(i).isSameStyleAs(s)){
            insertCloseTag = true;
            break;
        }
    }
    for(i=end+1; i<_symbols.size(); i++){
        if(_symbols.at(i).isSameStyleAs(s)){
            break;
        }
        if(_symbols.at(i).isOpeningOf(s)){
            insertOpenTag = true;
            break;
        }
    }
    if(!insertOpenTag && !insertCloseTag)
        return;
    //remove all -style- tags between end and start
    //start from last so we don't ruin indexes
    for(i=end; i>=start; i--){
        if(_symbols.at(i).isStyle() && Symbol::areSimilarTags(_symbols.at(i), s)){
            localErase(i);
            end--;
        }
    }
    if(insertOpenTag)
        localInsertStyle(end+1, s);
    if(insertCloseTag)
        localInsertStyle(start, Symbol::getClosedStyle(s));
    //eraseTwinTags();
}

void SharedEditor::localSetComplexStyle(int start, int end, Symbol s){
    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end);
    bool insertOpenTag = true, insertCloseTag = true;
    int i;
    Symbol openSymbol, closeSymbol;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        if(_symbols.at(i).isSameStyleAs(s)){
            insertOpenTag = false;
            break;
        }
        if(Symbol::areSimilarTags(_symbols.at(i), s)){
            openSymbol = _symbols.at(i);
            break;
        }

    }
    for(i=end; i<_symbols.size(); i++){
        if(Symbol::areSimilarTags(_symbols.at(i), s)){
            closeSymbol = _symbols.at(i);
            break;
        }
        if(_symbols.at(i).isOpeningOf(s)){
            insertCloseTag = false;
            break;
        }
    }
    if(!insertOpenTag && !insertCloseTag)
        return;
    //remove all -style- tags between end and start
    //start from last so we don't ruin indexes
    for(i=end; i>=start; i--){
        if(_symbols.at(i).isStyle() && Symbol::areSimilarTags(_symbols.at(i), s)){
            localErase(i);
            end--;
        }
    }
    if(insertCloseTag){
        localInsertStyle(end, Symbol::getClosedStyle(s));
        localInsertStyle(end+1, Symbol::getOpenStyle(closeSymbol));
    }
    if(insertOpenTag){
        localInsertStyle(start, Symbol::getClosedStyle(openSymbol));
        localInsertStyle(start+1, s);
    }
}


void SharedEditor::localSetStyle(int start, int end, Symbol s){
    if(s.isSimpleStyle())
        localSetSimpleStyle(start, end, s);
    else
        localSetComplexStyle(start, end, s);
}

void SharedEditor::localSetAlignment(int position, AlignmentType a){
    int realIndex = editorToVectorIndex(position);
    int lastPar;
    Symbol s;
    if(this->textCursor().hasSelection()){
        int start = editorToVectorIndex(this->textCursor().selectionStart());
        int end = editorToVectorIndex(this->textCursor().selectionEnd());
        for(lastPar=end; lastPar>=start && lastPar>=0; lastPar--){
            //for each paragraph in thee selection
            s = _symbols.at(lastPar);
            if(s.isStyle() && s.getStyleType() == StyleType::Paragraph){
                if(s.getAlignment() != a){
                    localErase(lastPar);
                    localInsertStyle(lastPar, Symbol(StyleType::Paragraph, a, _siteId, _counter, std::vector<int>()));
                }
            }
        }
        realIndex = lastPar;
    }
    //Cannot be < 0 since _symbols[0] is always a paragraph
    for(lastPar=realIndex; lastPar>=0; lastPar--){
        s = _symbols.at(lastPar);
        if(s.isStyle() && s.getStyleType() == StyleType::Paragraph){
            if(s.getAlignment() != a){
                localErase(lastPar);
                localInsertStyle(lastPar, Symbol(StyleType::Paragraph, a, _siteId, _counter, std::vector<int>()));
            }
            break;
        }
    }
}

void SharedEditor::clear(){
    _symbols.clear();
    QTextEdit::clear();
    _counter = 0;
    localInsertStyle(0, Symbol(StyleType::Paragraph, AlignmentType::AlignLeft, _siteId, _counter, std::vector<int>()));
    localInsertStyle(1, Symbol(StyleType::Font, "Arial", _siteId, _counter, std::vector<int>()));
    localInsertStyle(2, Symbol(StyleType::FontSize, 8, _siteId, _counter, std::vector<int>()));
    localInsertStyle(3, Symbol(StyleType::Color, "#000000", _siteId, _counter, std::vector<int>()));
    localInsert(0, '\0');
    localInsertStyle(5, Symbol(StyleType::FontEnd, "Arial", _siteId, _counter, std::vector<int>()));
    localInsertStyle(6, Symbol(StyleType::FontSizeEnd, 8, _siteId, _counter, std::vector<int>()));
    localInsertStyle(7, Symbol(StyleType::ColorEnd, "#000000", _siteId, _counter, std::vector<int>()));
}

void SharedEditor::localErase(int index) {
    if(index < 0 || index >= _symbols.size())
        return;
    Symbol s = _symbols.at(index);
    _symbols.erase(_symbols.begin()+index);
    //super inefficient, but should work at least for testing
    while(eraseTwinTags() > 0);
    Message m {MessageType::Erase, s};
    //EMISSION OF SIGNAL SHOULD HAPPEN IN MESSAGE QUEUE, BY ANOTHER THREAD
    emit packetReady(m); //DEBUG TESTING, NEEDS TO BE ADDED TO STYLE TOO
    //_mqOut.push(m);
}

void SharedEditor::incomingPacket(Message m){
    qDebug() << "A new packet arrived! Message is the following.";
    qDebug() << QString::fromStdString(m.toString());
    process(m);
}


//NOT WORKING PROPERLY
//TODO: FIX THIS ASAP AND TEST IT WITH MULTIPLE EDITORS
void SharedEditor::process(const Message &m) {
    int l=0, r=_symbols.size()-1, middle=-1;
    if(m.getType() == MessageType::Insert){
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
        if(l<=r) //Symbol found (already inserted, idempotency)
            return;
        QTextCursor cursor(this->textCursor());
        if(middle == -1){ //empty vector, never entered while cycle
            _symbols.insert(_symbols.begin(), m.getSymbol());
            cursor.atStart();
        }
        else if (m.getSymbol() < _symbols.at(middle)){
            _symbols.insert(_symbols.begin()+middle, m.getSymbol());
            cursor.setPosition(vectorToEditorIndex(middle));
        } else {
            _symbols.insert(_symbols.begin()+middle+1, m.getSymbol());
            cursor.setPosition(vectorToEditorIndex(middle+1));
        }
        if(m.getSymbol().isContent()){
            wchar_t content = m.getSymbol().getContent();
            wchar_t arr[2];
            arr[0] = content;
            arr[1] = '\0';
            cursor.insertText(QString::fromWCharArray(arr));
        }
    } else if (m.getType() == MessageType::Erase) {
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
        if(m.getSymbol().isContent()){
            QTextCursor cursor(this->textCursor());
            cursor.setPosition(middle);
            cursor.deleteChar();
        }
    }
}

std::wstring SharedEditor::to_string() {
    std::wstring result;
    auto it = _symbols.begin();
    for( ; it != _symbols.end() ; ++it){
        if((*it).isContent()){
            if(!(*it).getContent()=='\0')
                result.push_back((*it).getContent());
        }
        else{
            std::wstring tmp((*it).getTag().length(), L' ');
            std::string base((*it).getTag());
            std::copy(base.begin(), base.end(), tmp.begin());
            result.append(std::wstring(tmp));
        }
    }
    return result;
}

void SharedEditor::setSiteId(int id) {
    _siteId = id;
    _lseq.setSiteId(id);
}

void SharedEditor::setExampleSiteId(){
    setSiteId(1);
}
//super inefficient, but should work for testing purposes; needs to be run multiple times
int SharedEditor::eraseTwinTags(){
    int count = 0;
    for(int i = 0; i < _symbols.size(); i++){
        if(_symbols.at(i).isContent())
            continue;
        for(int j = i; j < _symbols.size(); j++){
            //empty tags have no content between them
            if(_symbols.at(j).isContent())
                break;
            //if there is no text between the two tags
            if(Symbol::areTwinTags(_symbols.at(i), _symbols.at(j))){
                //two twin tags
                localErase(j);
                localErase(i);
                count++;
            } else if(Symbol::areTwinTags(_symbols.at(i), _symbols.at(j))
                      && _symbols.at(j).getStyleType() == StyleType::Paragraph){
                //two consecutive paragraphs, remove the first one
                localErase(i);
                count++;
            }
        }
    }
    return count;
}

void SharedEditor::eraseSelectedText(QKeyEvent* e){
    if(e->key() == Qt::Key_Backspace){
        if(this->textCursor().selectionStart() != this->textCursor().selectionEnd()){
            for(int i = this->textCursor().selectionEnd()-1; i >= this->textCursor().selectionStart(); i--){
                localErase(editorToVectorIndex(i));
            }
        }
        else if(!this->textCursor().atStart()){
            localErase(editorToVectorIndex(this->textCursor().position()-1));
        }
    }
    else if(e->key() == Qt::Key_Delete){
        if(this->textCursor().selectionStart() != this->textCursor().selectionEnd()){
            for(int i = this->textCursor().selectionEnd()-1; i >= this->textCursor().selectionStart(); i--){
                localErase(editorToVectorIndex(i));
            }
        }
        else if(!this->textCursor().atEnd()){
            localErase(editorToVectorIndex(this->textCursor().position()));
        }
    }
    else {
        if(this->textCursor().selectionStart() != this->textCursor().selectionEnd()){
            for(int i = this->textCursor().selectionEnd()-1; i >= this->textCursor().selectionStart(); i--){
                localErase(editorToVectorIndex(i));
            }
        }
    }
}
