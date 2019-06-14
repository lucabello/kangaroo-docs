#include "qsharededitor.h"
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

QSharedEditor::QSharedEditor(QWidget *parent) : QTextEdit(parent){}

QSharedEditor::QSharedEditor(const QString &text, QWidget *parent) : QTextEdit(text, parent){}

QSharedEditor::~QSharedEditor(){}


void QSharedEditor::mergeCurrentCharFormat(const QTextCharFormat &modifier){
    QTextEdit::mergeCurrentCharFormat(modifier);
    qDebug() << "mergeCurrentCharFormat";
    //add message to _mqOut with MessageType::FORMAT + startSymbol, endSymbol + modifier
}

void QSharedEditor::setCurrentCharFormat(const QTextCharFormat &format){
    QTextEdit::setCurrentCharFormat(format);
    qDebug() << "setCurrentCharFormat";
    //add message to _mqOut with MessageType::FORMAT + startSymbol, endSymbol + modifier
}

void QSharedEditor::setAutoFormatting(AutoFormatting features){
    QTextEdit::setAutoFormatting(features);
    qDebug() << "setAutoFormatting";
}

void QSharedEditor::setTabChangesFocus(bool b){
    QTextEdit::setTabChangesFocus(b);
    qDebug() << "setTabChangesFocus";
}

void QSharedEditor::setLineWrapMode(LineWrapMode mode){
    QTextEdit::setLineWrapMode(mode);
    qDebug() << "setLineWrapMode";
}

void QSharedEditor::setLineWrapColumnOrWidth(int w){
    QTextEdit::setLineWrapColumnOrWidth(w);
    qDebug() << "setLineWrapColumnOrWidth";
}

void QSharedEditor::setWordWrapMode(QTextOption::WrapMode policy){
    QTextEdit::setWordWrapMode(policy);
    qDebug() << "setWordWrapMode";
}

void QSharedEditor::setOverwriteMode(bool overwrite){
    QTextEdit::setOverwriteMode(overwrite);
    qDebug() << "setOverwriteMode";
}

void QSharedEditor::setTabStopWidth(int width){
    QTextEdit::setTabStopWidth(width);
    qDebug() << "setTabStopWidth";
}

void QSharedEditor::setTabStopDistance(qreal distance){
    QTextEdit::setTabStopDistance(distance);
    qDebug() << "setTabStopDistance";
}

void QSharedEditor::setCursorWidth(int width){
    QTextEdit::setCursorWidth(width);
    qDebug() << "setCursorWidth";
}

void QSharedEditor::setAcceptRichText(bool accept){
    QTextEdit::setAcceptRichText(accept);
    qDebug() << "setAcceptRichText";
}

void QSharedEditor::setExtraSelections(const QList<ExtraSelection> &selections){
    QTextEdit::setExtraSelections(selections);
    qDebug() << "setExtraSelections";
}

void QSharedEditor::moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode){
    QTextEdit::moveCursor(operation, mode);
    qDebug() << "moveCursor";
}

void QSharedEditor::updatePreviousFields(){
    this->previousAnchor = this->tempAnchor;
    this->previousPosition = this->tempPosition;
    this->previousSelection = this->tempSelection;
    this->previousLChar = this->tempLChar;
    this->previousRChar = this->tempRChar;
    this->tempAnchor = this->textCursor().anchor();
    this->tempPosition = this->textCursor().position();
    this->tempSelection = this->textCursor().selectedText();
    if(this->textCursor().position() > 0)
        this->tempLChar = this->toPlainText().at(this->textCursor().position()-1);
    else
        this->tempLChar = "";
    if(this->textCursor().position() < this->toPlainText().length())
        this->tempRChar = this->toPlainText().at(this->textCursor().position());
    else
        this->tempRChar = "";
}

bool isKeyPrintable(QKeyEvent * e){
    int k = e->key();
    /*
    int modifiers = e->modifiers();
    if(modifiers == Qt::ControlModifier){
        return false;
    }
    if(!(k == Qt::Key_Tab || k == Qt::Key_Backtab ||
            k == Qt::Key_Return	||
            k == Qt::Key_Enter ||
            (k >= 0x20 && k <= 0xdf) ||
            k == Qt::Key_division ||
            k == Qt::Key_ydiaeresis ||
            k == 0))
        return false;
    if(e->text().length() != 1)
        return false;*/
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        return true;
    int category = e->text().unicode()[0].category();
    qDebug() << "CATEGORY = " << category;
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

void QSharedEditor::keyPressEvent(QKeyEvent * e){
    if(e->key() == Qt::Key_0){
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << "Debug key pressed.";
        qDebug() << "Hope for the best. Good luck.";
        eraseTwinTags();
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << to_string();
        qDebug() << "++++++++++++++++++++++++++++";
        return;
    }
    qDebug() << "KEY PRESS CAUGHT YAY";
    qDebug() << "KEY = " << e->key();
    if(isKeyPrintable(e)){
        qDebug() << "- Printable key: " << e->text() << " - key: " << e->key();
        wchar_t c[1];
        e->text().toWCharArray(c);
        localInsert(this->textCursor().position(), c[0]);
        //emit packetReady(e->text()); //only for testing. separate thread should already send packet
    }
    else if(isKeyPaste(e)){
        //qDebug() << "- You pasted: " << QApplication::clipboard()->mimeData()->html();
        return;
    }
    QTextEdit::keyPressEvent(e);
}











// SharedEditor methods

int QSharedEditor::realIndex(int i){
    int a=0, count=0;
    for(; a <= i+count && a < _symbols.size() && !_symbols.empty(); a++){
        if(_symbols.at(a).isStyle()){
            count++;
        }
    }
    return count+i;
}

void QSharedEditor::localInsert(int index, wchar_t value) {
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    index = realIndex(index); //sum number of tag symbols to index
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
    //Message m {MessageType::INSERT, s}; //prepare and send message
    //_mqOut.push(m);
}

void QSharedEditor::localInsertStyle(int index, Symbol styleSymbol){
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    prev = (index-1 >= 0 && index-1 < _symbols.size())?
            _symbols.at(index-1).getPosition() : prev;
    succ = (index >= 0 && index < _symbols.size())?
            _symbols.at(index).getPosition() : succ;
    Symbol s {styleSymbol, _siteId, _counter, _lseq.alloc(prev, succ)};
    _counter++;
    _symbols.insert(_symbols.begin()+index, s);
    //Message m {MessageType::INSERT, s}; //prepare and send message
    //_mqOut.push(m);
}

//start and end are included
//TODO: DOES NOT WORK CORRECTLY FOR COLOR AND SIMILAR TAGS.
void QSharedEditor::localSetStyle(int start, int end, Symbol s){
    start = realIndex(start);
    end = realIndex(end);
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

//start and end are included
void QSharedEditor::localUnsetStyle(int start, int end, Symbol s){
    start = realIndex(start);
    end = realIndex(end)-1;
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

void QSharedEditor::clear(){
    _symbols.clear();
    QTextEdit::clear();
    _counter = 0;
    localInsertStyle(0, Symbol(StyleType::Paragraph, AlignmentType::AlignLeft, _siteId, _counter, std::vector<int>()));
    localInsertStyle(1, Symbol(StyleType::Font, "Arial", _siteId, _counter, std::vector<int>()));
    localInsertStyle(2, Symbol(StyleType::FontSize, 8, _siteId, _counter, std::vector<int>()));
    localInsertStyle(3, Symbol(StyleType::Color, "000000", _siteId, _counter, std::vector<int>()));
    localInsert(0, '\0');
    localInsertStyle(5, Symbol(StyleType::FontEnd, "Arial", _siteId, _counter, std::vector<int>()));
    localInsertStyle(6, Symbol(StyleType::FontSizeEnd, 8, _siteId, _counter, std::vector<int>()));
    localInsertStyle(7, Symbol(StyleType::ColorEnd, "000000", _siteId, _counter, std::vector<int>()));
}

void QSharedEditor::localErase(int index) {
    if(index < 0 || index >= _symbols.size())
        return;
    Symbol s = _symbols.at(index);
    _symbols.erase(_symbols.begin()+index);
    //eraseTwinTags();
    //Message m {MessageType::ERASE, s};
    //_mqOut.push(m);
}

//to correct
void QSharedEditor::process(const Message &m) {
//    int l=0, r=_symbols.size()-1, middle=-1;
//    if(m.getType() == MessageType::INSERT){
//        /*
//         * After this binary search, the variable middle can be:
//         * - -1 if the vector is empty
//         * - index of the first element bigger than s
//         * - index of the first element smaller than s
//         * If the symbol is found, the function will return.
//         */
//        while(l<=r){
//            middle = (l+r)/2;
//            if(_symbols.at(middle) < m.getSymbol())
//                l = middle+1;
//            else if(m.getSymbol() < _symbols.at(middle))
//                r = middle-1;
//            else
//                break;
//        }
//        if(l<=r) //Symbol found (already inserted)
//            return;
//        if(middle == -1) //empty vector, never entered while cycle
//            _symbols.insert(_symbols.begin(), m.getSymbol());
//        else if (m.getSymbol() < _symbols.at(middle))
//            _symbols.insert(_symbols.begin()+middle, m.getSymbol());
//        else
//            _symbols.insert(_symbols.begin()+middle+1, m.getSymbol());
//    } else if (m.getType() == MessageType::ERASE) {
//        //middle variable is modified or the function returns
//        while(l<=r){
//            middle = (l+r)/2;
//            if(_symbols.at(middle) < m.getSymbol())
//                l = middle+1;
//            else if(m.getSymbol() < _symbols.at(middle))
//                r = middle-1;
//            else
//                break;
//        }
//        if(l > r) //Symbol not found (already deleted or empty vector)
//            return;
//        _symbols.erase(_symbols.begin()+middle);
//    }
}

std::wstring QSharedEditor::to_string() {
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

void QSharedEditor::setSiteId(int id) {
    _siteId = id;
    _lseq.setSiteId(id);
}

//super inefficient, but should work for testing purposes
void QSharedEditor::eraseTwinTags(){
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
            } else if(Symbol::areTwinTags(_symbols.at(i), _symbols.at(j))
                      && _symbols.at(j).getStyleType() == StyleType::Paragraph){
                //two consecutive paragraphs, remove the first one
                localErase(i);
            }
        }
    }
}
