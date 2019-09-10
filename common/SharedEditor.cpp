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

bool isKeyCopy(QKeyEvent * e){
    //copy is in QChar category 9! change this function and test it
    int k = e->key();
    int modifiers = e->modifiers();
    if(modifiers == Qt::ControlModifier && k == Qt::Key_C)
        return true;
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
        for(Symbol s : _symbols){
            qDebug() << "[key0]" << QString::fromStdString(s.toString());
        }
        qDebug() << "++++++++++++++++++++++++++++";
        return;
    }
    if(e->key() == Qt::Key_8){
        qDebug() << "++++++++++++++++++++++++++++";
        qDebug() << "Printing symbols...";
        qDebug() << "++++++++++++++++++++++++++++";
        for(Symbol s : _symbols){
            qDebug() << "[key8]" << QString::fromStdString(s.toString());
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
    }/*
    else if(isKeyCopy(e)){
        QTextCursor cursor(this->textCursor());
        if(!cursor.hasSelection())
            return;
        _clipboard.clear();
        int start = editorToVectorIndex(cursor.selectionStart());
        int end = editorToVectorIndex(cursor.selectionEnd());
        bool font=true, fontsize=true, color=true, bold=true, italic=true, underlined=true;
        for(int i=start-1; i>=0; i--){
            Symbol s = _symbols.at(i);
            if(s.isContent())
                continue;
            if(s.getStyleType() == StyleType::Paragraph)
                continue;
            if(s.getStyleType() == StyleType::Bold && bold == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::BoldEnd)
                bold = false;
            else if(s.getStyleType() == StyleType::Italic && italic == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::ItalicEnd)
                italic = false;
            else if(s.getStyleType() == StyleType::Underlined && underlined == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::UnderlinedEnd)
                underlined = false;
            else if(s.getStyleType() == StyleType::Font && font == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::FontEnd)
                font = false;
            else if(s.getStyleType() == StyleType::FontSize && fontsize == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::FontSizeEnd)
                fontsize = false;
            else if(s.getStyleType() == StyleType::Color)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::ColorEnd)
                color = false;
        }
        for(int i=start; i<end; i++)
            _clipboard.push_back(_symbols.at(i));
        font=true, fontsize=true, color=true, bold=true, italic=true, underlined=true;
        for(int i=end; i<_symbols.size(); i++){
            Symbol s = _symbols.at(i);
            if(s.isContent())
                continue;
            if(s.getStyleType() == StyleType::Paragraph)
                continue;
            if(s.getStyleType() == StyleType::BoldEnd && bold == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::Bold)
                bold = false;
            else if(s.getStyleType() == StyleType::ItalicEnd && italic == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::Italic)
                italic = false;
            else if(s.getStyleType() == StyleType::UnderlinedEnd && underlined == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::Underlined)
                underlined = false;
            else if(s.getStyleType() == StyleType::FontEnd && font == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::Font)
                font = false;
            else if(s.getStyleType() == StyleType::FontSizeEnd && fontsize == true)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::FontSize)
                fontsize = false;
            else if(s.getStyleType() == StyleType::ColorEnd)
                _clipboard.push_back(s);
            else if(s.getStyleType() == StyleType::Color)
                color = false;
        }
        return;
    }
    else if(isKeyPaste(e)){
        //qDebug() << "- You pasted: " << QApplication::clipboard()->mimeData()->html();
        qDebug() << "[SharedEditor] Clipboard: ";
        int editorPosition = this->textCursor().position();
        int editorStartPosition = editorPosition;
        int position = editorToVectorIndex(this->textCursor().position());
        int startPosition = position;
        wchar_t content[1];
        for(Symbol s : _clipboard){
            qDebug() << "[clip]" << QString::fromStdString(s.toString());
            if(s.isContent()){
                content[0] = s.getContent();
                localInsert(position, s.getContent());
                this->insertPlainText(QString::fromWCharArray(content));
                position++;
                editorPosition++;
            }
        }
        int count = 0;
        int internalCount = 0;
        for(int i=0; i<_clipboard.size(); i++){
            internalCount=0;
            Symbol open = _clipboard.at(i);
            if(open.isContent())
                count++;
            if(!open.isStyle() || !open.isOpenTag())
                continue;
            for(int j=i+1; j<_clipboard.size(); j++){
                Symbol close = _clipboard.at(j);
                if(close.isContent()){
                    internalCount++;
                    continue;
                }
                if(open.isOpeningOf(close)){
                    localSetStyle(editorStartPosition+count, editorStartPosition+internalCount, open);
                    break;
                }
            }
        }
        applyStylesToEditor();
        return;
    }*/
    else if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right || e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {

    }
    else if(e->text().unicode()[0].category() == 9){
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

    qDebug() << count << "," << count+i;
    qDebug() << QString::fromStdString(_symbols.at(count+i).toString()) << " count + i";
    qDebug() << QString::fromStdString(_symbols.at(count+i-1).toString()) << " count + i - 1";

    return count+i;
}

int SharedEditor::vectorToEditorIndex(int i){
    int a=0, count=0;
    for(; a < i && a < _symbols.size() && !_symbols.empty(); a++){
        if(_symbols.at(a).isStyle()){
            count++;
        }
        else if(_symbols.at(a).isContent() && _symbols.at(a).isFake()){
            count++;
        }
    }
    return i-count;
}

void SharedEditor::avoidBackgroundPropagation(int editorIndexSym){
    if(editorIndexSym>=0&&!siteIdToCursor.empty()){
        int indexSym=editorToVectorIndex(editorIndexSym);
        //qDebug()<<indexSym<<"<"<<_symbols.size();
        if(indexSym >= 0 && indexSym < _symbols.size()){
            Symbol prevSym=_symbols.at(indexSym);
            std::map<qint32,Symbol>::iterator mapIt;
            for(mapIt=siteIdToCursor.begin();((!(mapIt->second==prevSym))||mapIt->first==_siteId)&&mapIt!=siteIdToCursor.end();++mapIt);
            if(mapIt!=siteIdToCursor.end()){
                //qDebug()<<"No propagation position:"<<editorIndexSym <<"-"<<editorIndexSym+1;
                QTextCursor cursor=this->textCursor();
                int tmp=cursor.position();
                cursor.setPosition(editorIndexSym, QTextCursor::MoveAnchor);
                cursor.setPosition(editorIndexSym+1, QTextCursor::KeepAnchor);
                this->setTextCursor(cursor);
                QTextCharFormat fmt=QTextCharFormat();
                fmt.clearBackground();
                fmt.setBackground(QColor("#ffffff"));
                mergeCurrentCharFormat(fmt);
                cursor.setPosition(tmp);
                this->setTextCursor(cursor);
            }
        }
    }
}

void SharedEditor::localInsert(int index, wchar_t value) {
    std::vector<int> prev, succ;
    //get prev and succ positions if present
    int editorIndexSym=index-1;
    int i;

    qDebug() << editorIndexSym << " editorIndexSym";
    qDebug() << index << "index";

    index = editorToVectorIndex(index); //sum number of tag symbols to index

    //allow propagation of style
    for(i=1; editorIndexSym!=-1 && index-i>0 && _symbols.at(index-i).isStyle(); i++);

    i--;

    if(i>0)
        index-=i;

    qDebug() << index << "indexFixedVector";
    qDebug() << _symbols.size() << "_symbolsSize";

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

    //AVOID BACKGROUND PROPAGATION -- better if after insertText with editorIndexSym=index
    avoidBackgroundPropagation(editorIndexSym);

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
    qDebug() << QString::fromStdString(s.toString()) << " inserting symbol";
    _symbols.insert(_symbols.begin()+index, s);
    Message m {MessageType::Insert, s}; //prepare and send message
    //EMISSION OF SIGNAL SHOULD HAPPEN IN MESSAGE QUEUE, BY ANOTHER THREAD
    emit packetReady(m); //DEBUG TESTING, NEEDS TO BE ADDED TO STYLE TOO
    //_mqOut.push(m);
}

void SharedEditor::localSetSimpleStyle(int start, int end, Symbol s){

//    qDebug() << "start :" << start;
//    qDebug() << "end :" << end;

    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end);
    bool insertOpenTag = true, insertCloseTag = true;

//    qDebug() << "start :" << start;
//    qDebug() << "end :" << end;

    int i;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        if(_symbols.at(i).isClosingOf(s)){
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
        if(_symbols.at(i).isClosingOf(s)){
            insertCloseTag = false;
            break;
        }
    }

    qDebug() << "insertOpenTag :" << insertOpenTag;
    qDebug() << "insertCloseTag :" << insertCloseTag;

    //this should never happen
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

//    qDebug() << "start :" << start;
//    qDebug() << "end :" << end;

    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end)-1;

    qDebug() << "start :" << start << QString::fromStdString(_symbols.at(start).toString());
    qDebug() << "end :" << end << QString::fromStdString(_symbols.at(end).toString());

    bool insertOpenTag = false, insertCloseTag = false;
    int i;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        //if I find a closing tag (isClosingOf) before an opening one (isSameStyleAs)
        //it means that my start is not in the middle of a style tag
        //otherwise I am in the middle and I have to add a closing style tag in start position
        if(_symbols.at(i).isClosingOf(s)){
            break;
        }
        if(_symbols.at(i).isSameStyleAs(s)){
            insertCloseTag = true;
            break;
        }
    }
    for(i=end+1; i<_symbols.size(); i++){
        //if I find an opening tag (isSameStyleAs) before a closing one (isClosingOf)
        //it means that my end is not in the middle of a style tag
        //otherwise I am in the middle and I have to add a opening style tag in end position
        if(_symbols.at(i).isSameStyleAs(s)){
            break;
        }
        if(_symbols.at(i).isClosingOf(s)){
            insertOpenTag = true;
            break;
        }
    }

//    qDebug() << "insertOpenTag :" << insertOpenTag;
//    qDebug() << "insertCloseTag :" << insertCloseTag;

    //this should never happen
    if(!insertOpenTag && !insertCloseTag)
        return;

    //remove all -style- tags between end and start
    //start from last so we don't ruin indexes
    for(i=end+1; i>=start; i--){
        if(_symbols.at(i).isStyle() && Symbol::areSimilarTags(_symbols.at(i), s)){
            localErase(i);
            end--;
        }
    }
    if(insertOpenTag)
        localInsertStyle(end+1, s);
    if(insertCloseTag)
        localInsertStyle(start, Symbol::getClosedStyle(s));
    while(eraseTwinTags()>0);
}

void SharedEditor::localSetComplexStyle(int start, int end, Symbol s){
    start = editorToVectorIndex(start);
    end = editorToVectorIndex(end);

    qDebug() << "start :" << start << QString::fromStdString(_symbols.at(start).toString());
    qDebug() << "end :" << end << QString::fromStdString(_symbols.at(end).toString());

    bool insertOpenTag = true, insertCloseTag = true;
    int i;
    Symbol openSymbol, closeSymbol;
    //check if we are inside a -style- tag
    for(i=start-1; i>=0; i--){
        //check if we find an opening tag of the style first. If we do it means we are already in a -style- tag
        //regarding the same exact color/font/size so we don't need an opening tag
        if(_symbols.at(i).isSameStyleAs(s)){
            insertOpenTag = false;
            break;
        }
        if(Symbol::areSimilarTags(_symbols.at(i), s)){
            //keep track of the style tag we have to close if the style is similar but different color/font/size
            openSymbol = _symbols.at(i);
            break;
        }
    }
    for(i=end; i<_symbols.size(); i++){
        if(Symbol::areSimilarTags(_symbols.at(i), s)){
            //keep track of the style tag we have to close if the style is similar but different color/font/size
            closeSymbol = _symbols.at(i);
            break;
        }
        if(_symbols.at(i).isClosingOf(s)){
            //check if we find a closing tag of the style first. If we do it means we are already in a -style- tag
            //regarding the same exact color/font/size so we don't need a closing tag
            insertCloseTag = false;
            break;
        }
    }

    //this should never happen
    if(!insertOpenTag && !insertCloseTag)
        return;
    //remove all -style- tags between end and start that are of the similar
    //start from last so we don't ruin indexes
    for(i=end; i>=start; i--){
        if(_symbols.at(i).isStyle() && Symbol::areSimilarTags(_symbols.at(i), s)){
            localErase(i);
            end--;
        }
    }

    qDebug() << "start :" << start << QString::fromStdString(_symbols.at(start).toString());
    qDebug() << "end :" << end << QString::fromStdString(_symbols.at(end).toString());

    if(insertCloseTag){
        localInsertStyle(end, Symbol::getClosedStyle(s));
        localInsertStyle(end+1, Symbol::getOpenStyle(closeSymbol));
    }
    if(insertOpenTag){
        localInsertStyle(start, Symbol::getClosedStyle(openSymbol));
        localInsertStyle(start+1, s);
    }

    while(eraseTwinTags()>0);
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
    QTextCharFormat fmt;
    fmt = QTextCharFormat();
    fmt.setFontItalic(false);
    fmt.setFontWeight(QFont::Normal);
    fmt.setFontUnderline(false);
    mergeCurrentCharFormat(fmt);
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
    //qDebug() << "[SharedEditor] New Message arrived.";
    //qDebug() << QString::fromStdString(m.toString());
    if(m.getType() == MessageType::Insert ||
            m.getType() == MessageType::Erase)
        process(m);
    else if(m.getType() == MessageType::EditorList){
        setEditorList(m.getCommand());
    }else if(m.getType()==MessageType::FileSent){
        hideUserCursors();
    }
}


//NOT WORKING PROPERLY
//TODO: FIX THIS ASAP AND TEST IT WITH MULTIPLE EDITORS
void SharedEditor::process(const Message &m) {
    if(m.getType() == MessageType::Insert){
        Symbol sym = m.getSymbol();

        qint32 siteId=sym.getSiteId();
        QTextCursor cursor=textCursor();

        //if symbol found (already inserted) return
        if(std::find(_symbols.begin(), _symbols.end(), m.getSymbol()) != _symbols.end())
            return;
        auto it = _symbols.begin();
        for(; it != _symbols.end(); ++it){
            if(sym < *it)
                break;
        }
        int index = std::distance(_symbols.begin(), it);
        _symbols.insert(it, sym);
        cursor.setPosition(vectorToEditorIndex(index));
        if(m.getSymbol().isContent()){
            wchar_t content = sym.getContent();
            wchar_t arr[2];
            arr[0] = content;
            arr[1] = '\0';
            if(m.getSymbol().getSiteId() == 0 && m.getSymbol().isFake()){
                return;
            }
            int cursorStart=cursor.position();

            //AVOID BACKGROUND PROPAGATION -- better if after insertText without -1
            avoidBackgroundPropagation(cursorStart-1);

            cursor.insertText(QString::fromWCharArray(arr));
            int cursorEnd=cursor.position();

            //MOVE USER CURSOR
            moveUserCursor(cursorStart,cursorEnd,m);
        }
        else {
            qDebug() << "Inserting style tag";
            applyStylesToEditor();
        }
    } else if (m.getType() == MessageType::Erase) {
        auto it = std::find(_symbols.begin(), _symbols.end(), m.getSymbol());
        int index = std::distance(_symbols.begin(), it);
        if(it == _symbols.end()) //Symbol not found (already deleted or empty vector)
            return;
        _symbols.erase(it);
        if(m.getSymbol().isContent()){
            QTextCursor cursor(this->textCursor());
            cursor.setPosition(vectorToEditorIndex(index));

            int cursorStart=cursor.position()-1;
            cursor.deleteChar();
            int cursorEnd=cursor.position();

            //MOVE USER CURSOR
            moveUserCursor(cursorStart,cursorEnd,m);
        }
        else {
            qDebug() << "Erasing style tag";
            applyStylesToEditor();
        }
    }
}

QColor SharedEditor::randomColor(){
    //Between 0 and 240, to avoid black and white
    return QColor(rand()%231+10,rand()%231+10,rand()%231+10);
}

bool SharedEditor::siteIdHasColor(qint32 siteId){
    return siteIdToColor.find(siteId)!=siteIdToColor.end();
}

QColor SharedEditor::getSiteIdColor(qint32 siteId){
    return siteIdToColor[siteId];
}

void SharedEditor::moveUserCursor(int cursorStart,int cursorEnd,Message m){
    Symbol sym = m.getSymbol();
    qint32 siteId=sym.getSiteId();
    QTextCursor cursor=textCursor();
    int originalPosition=cursor.position();

    if(siteId==_siteId)
        return;

    if(!usernameToSiteId.empty()){
        std::map<QString,int>::iterator mapIt;
        for(mapIt=usernameToSiteId.begin();mapIt->second!=siteId&&mapIt!=usernameToSiteId.end();++mapIt);
        if(mapIt==usernameToSiteId.end())
            return;
    }else
        return;

    QTextCharFormat fmt=QTextCharFormat();
    std::vector<Symbol>::iterator it;
    if(siteIdToCursor.find(siteId)==siteIdToCursor.end()){
        siteIdToCursor.insert(std::pair<qint32,Symbol>(siteId,sym));
        if(!siteIdHasColor(siteId))
            siteIdToColor.insert(std::pair<qint32,QColor>(siteId,randomColor()));
    }
    else if((it=std::find(_symbols.begin(), _symbols.end(), siteIdToCursor[siteId])) !=_symbols.end()){
        int index = vectorToEditorIndex(std::distance(_symbols.begin(),it));
        //qDebug()<<"previous position:"<<index <<"-"<<index+1;
        cursor.setPosition(index, QTextCursor::MoveAnchor);
        cursor.setPosition(index+1, QTextCursor::KeepAnchor);
        this->setTextCursor(cursor);
        fmt.clearBackground();
        fmt.setBackground(QColor("#ffffff"));
        mergeCurrentCharFormat(fmt);
    }
    if(m.getType()!=MessageType::Erase){
        siteIdToCursor[siteId]=sym;
        cursor.setPosition(cursorStart, QTextCursor::MoveAnchor);
        cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
        this->setTextCursor(cursor);
        fmt.setBackground(siteIdToColor[siteId]);
        mergeCurrentCharFormat(fmt);
    }else{
        if(cursorEnd>0){
            siteIdToCursor[siteId]=_symbols.at(editorToVectorIndex(cursorStart));
            cursor.setPosition(cursorStart, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(cursor);
            fmt.setBackground(siteIdToColor[siteId]);
            mergeCurrentCharFormat(fmt);
        }else
            siteIdToCursor.erase(siteId);
    }

    //qDebug()<<"after cursor:"<< cursorStart << "-" <<cursorEnd;

    cursor.setPosition(originalPosition);
    setTextCursor(cursor);
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
            std::string base((*it).getTag().toStdString());
            std::copy(base.begin(), base.end(), tmp.begin());
            result.append(std::wstring(tmp));
        }
    }
    return result;
}

int SharedEditor::getSiteId(){
    return _siteId;
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
        for(int j = i+1; j < _symbols.size(); j++){
            //empty tags have no content between them
            if(_symbols.at(j).isContent()){
                qDebug()<<"Here1";
                break;
            }
            //if there is no text between the two tags
            if(Symbol::areTwinTags(_symbols.at(i), _symbols.at(j))
                && _symbols.at(j).getStyleType() == StyleType::Paragraph){
                //two consecutive paragraphs, remove the first one
                localErase(i);
                count++;
            }
            else if(Symbol::areTwinTags(_symbols.at(i), _symbols.at(j))){
                //two twin tags
                qDebug()<<"Here2";
                localErase(j);
                localErase(i);
                count++;
            }
        }
    }
    return count;
}

void SharedEditor::propagateStyleToEditor(int index){
    Symbol s = _symbols.at(index);
    QTextCursor cursor = this->textCursor();
    QTextCharFormat fmt;
    qDebug() << "[Propagate] Style:" << s.getTag();
    if(s.getStyleType() == StyleType::Paragraph){
        //find the first content symbol after paragraph
        for(int i = index; i < _symbols.size() && _symbols.at(i).isStyle(); i++);
        if(s.getAlignment() == AlignmentType::AlignLeft)
            setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        else if(s.getAlignment() == AlignmentType::AlignCenter)
            setAlignment(Qt::AlignCenter);
        else if(s.getAlignment() == AlignmentType::AlignRight)
            setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        else if(s.getAlignment() == AlignmentType::AlignJustified)
            setAlignment(Qt::AlignJustify);
    }
    else if(s.isOpenTag()){
        //look forward for the first closing tag
        for(int i = index; i < _symbols.size(); i++){
            if(s.isOpeningOf(_symbols.at(i))){
                //apply style
                int start = vectorToEditorIndex(index);
                int end = vectorToEditorIndex(i);
                cursor.setPosition(start, QTextCursor::MoveAnchor);
                cursor.setPosition(end, QTextCursor::KeepAnchor);
                if(s.getStyleType() == StyleType::Bold)
                    fmt.setFontWeight(QFont::Bold);
                else if(s.getStyleType() == StyleType::Italic)
                    fmt.setFontItalic(true);
                else if(s.getStyleType() == StyleType::Underlined)
                    fmt.setFontUnderline(true);
                else if(s.getStyleType() == StyleType::Font)
                    fmt.setFontFamily(s.getFontName());
                else if(s.getStyleType() == StyleType::FontSize)
                    fmt.setFontPointSize(s.getFontSize());
                else if(s.getStyleType() == StyleType::Color)
                    fmt.setForeground(QColor(s.getColor())); //to check for correctness
                break;
            }
            else if(s.isSameStyleAs(_symbols.at(i)))
                break;
        }
    }
    else if(s.isCloseTag()){
        //look backwards for the first opening tag
        for(int i = 0; i >= 0; i--){
            if(_symbols.at(i).isOpeningOf(s)){
                //apply style
                int start = vectorToEditorIndex(i);
                int end = vectorToEditorIndex(index);
                cursor.setPosition(start, QTextCursor::MoveAnchor);
                cursor.setPosition(end, QTextCursor::KeepAnchor);
                if(s.getStyleType() == StyleType::BoldEnd)
                    fmt.setFontWeight(QFont::Bold);
                else if(s.getStyleType() == StyleType::ItalicEnd)
                    fmt.setFontItalic(true);
                else if(s.getStyleType() == StyleType::UnderlinedEnd)
                    fmt.setFontUnderline(true);
                else if(s.getStyleType() == StyleType::FontEnd)
                    fmt.setFontFamily(s.getFontName());
                else if(s.getStyleType() == StyleType::FontSizeEnd)
                    fmt.setFontPointSize(s.getFontSize());
                else if(s.getStyleType() == StyleType::ColorEnd)
                    fmt.setForeground(QColor(s.getColor())); //to check for correctness
                break;
            }
            else if(s.isSameStyleAs(_symbols.at(i)))
                break;
        }
    }
}

void SharedEditor::applyStylesToEditor(){
    //search for Bolds
    bool bold = false;
    bool italic = false;
    bool underlined = false;
    bool paragraph = false;
    bool font = false;
    bool fontsize = false;
    bool color = false;
    int boldStart = -1, boldEnd = -1;
    int italicStart = -1, italicEnd = -1;
    int underlinedStart = -1, underlinedEnd = -1;
    int paragraphStart = -1;
    int fontStart = -1, fontEnd = -1;
    int fontsizeStart = -1, fontsizeEnd = -1;
    int colorStart = -1, colorEnd = -1;
    int lastBoldEnd=0;
    int lastItalicEnd=0;
    int lastUnderlinedEnd=0;
    int lastFontEnd=0;
    int lastFontsizeEnd=0;
    int lastColorEnd=0;
//  int lastStyleEnd=0;


    QTextCursor previousCursor(this->textCursor());
    QTextCursor currentCursor(this->textCursor());
    QTextCharFormat fmt;
    for(int i = 0; i < _symbols.size(); i++){
        fmt = QTextCharFormat();
        Symbol s = _symbols.at(i);
        if(!s.isStyle())
            continue;
        if(s.getStyleType() == StyleType::Paragraph){
            paragraph = true;
            //apply paragraph alignment to the paragraph
            paragraphStart = vectorToEditorIndex(i);
            currentCursor.setPosition(paragraphStart);
            this->setTextCursor(currentCursor);
            if(s.getAlignment() == AlignmentType::AlignLeft)
                setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
            else if(s.getAlignment() == AlignmentType::AlignCenter)
                setAlignment(Qt::AlignCenter);
            else if(s.getAlignment() == AlignmentType::AlignRight)
                setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
            else if(s.getAlignment() == AlignmentType::AlignJustified)
                setAlignment(Qt::AlignJustify);
        }
        else if(s.getStyleType() == StyleType::Bold){
            boldStart = vectorToEditorIndex(i);
            /*
            if(bold == false && boldEnd > -1){
                currentCursor.setPosition(boldEnd, QTextCursor::MoveAnchor);
                currentCursor.setPosition(boldStart, QTextCursor::KeepAnchor);
                this->setTextCursor(currentCursor);
                fmt.setFontWeight(QFont::Normal);
                mergeCurrentCharFormat(fmt);
            }
            */
            bold = true;
        }
        else if(s.getStyleType() == StyleType::BoldEnd && bold == true){
            boldEnd = vectorToEditorIndex(i);
            //apply style
            currentCursor.setPosition(boldStart, QTextCursor::MoveAnchor);
            qDebug() << "Start: " << boldStart;
            currentCursor.setPosition(boldEnd, QTextCursor::KeepAnchor);
            qDebug() << "End: " << boldEnd;
            this->setTextCursor(currentCursor);
            fmt.setFontWeight(QFont::Bold);
            currentCursor.mergeCharFormat(fmt);
            //remove it at the end of the cursor
            if(i < _symbols.size()-1){
                currentCursor.setPosition(boldEnd, QTextCursor::MoveAnchor);
                currentCursor.setPosition(vectorToEditorIndex(_symbols.size()-1), QTextCursor::KeepAnchor);
                this->setTextCursor(currentCursor);
                fmt.setFontWeight(QFont::Normal);
                currentCursor.mergeCharFormat(fmt);
            }
            currentCursor.setPosition(lastBoldEnd, QTextCursor::MoveAnchor);
            currentCursor.setPosition(boldStart-1, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontWeight(QFont::Normal);
            currentCursor.mergeCharFormat(fmt);
            qDebug() << "Last bold end: " << lastBoldEnd;
            qDebug() << "Bold start: " << boldStart;
            lastBoldEnd=boldEnd;
//            if(lastBoldEnd > lastStyleEnd)
//                lastStyleEnd=lastBoldEnd;
            bold = false;
        }
        else if(s.getStyleType() == StyleType::Italic){
            italic = true;
            italicStart = vectorToEditorIndex(i);
        }
        else if(s.getStyleType() == StyleType::ItalicEnd && italic == true){
            italicEnd = vectorToEditorIndex(i);
            //apply style
            currentCursor.setPosition(italicStart, QTextCursor::MoveAnchor);
            currentCursor.setPosition(italicEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontItalic(true);
            mergeCurrentCharFormat(fmt);
            //remove it at the end of the cursor
            if(i < _symbols.size()-1){
                currentCursor.setPosition(italicEnd, QTextCursor::MoveAnchor);
                currentCursor.setPosition(vectorToEditorIndex(_symbols.size()-1), QTextCursor::KeepAnchor);
                this->setTextCursor(currentCursor);
                fmt.setFontItalic(false);
                mergeCurrentCharFormat(fmt);
            }
            currentCursor.setPosition(lastItalicEnd, QTextCursor::MoveAnchor);
            currentCursor.setPosition(italicStart-1, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontItalic(false);
            currentCursor.mergeCharFormat(fmt);
            lastItalicEnd=italicEnd;
//            if(lastItalicEnd > lastStyleEnd)
//                lastStyleEnd=lastItalicEnd;
            italic = false;
        }
        else if(s.getStyleType() == StyleType::Underlined){
            underlined = true;
            underlinedStart = vectorToEditorIndex(i);
        }
        else if(s.getStyleType() == StyleType::UnderlinedEnd && underlined == true){
            underlinedEnd = vectorToEditorIndex(i);
            //apply style
            currentCursor.setPosition(underlinedStart, QTextCursor::MoveAnchor);
            currentCursor.setPosition(underlinedEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontUnderline(true);
            mergeCurrentCharFormat(fmt);
            //remove it at the end of the cursor
            if(i < _symbols.size()-1){
                currentCursor.setPosition(underlinedEnd, QTextCursor::MoveAnchor);
                currentCursor.setPosition(vectorToEditorIndex(_symbols.size()-1), QTextCursor::KeepAnchor);
                this->setTextCursor(currentCursor);
                fmt.setFontUnderline(false);
                mergeCurrentCharFormat(fmt);
            }
            currentCursor.setPosition(lastUnderlinedEnd, QTextCursor::MoveAnchor);
            currentCursor.setPosition(underlinedStart-1, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontUnderline(false);
            currentCursor.mergeCharFormat(fmt);
            lastUnderlinedEnd=underlinedEnd;
//            if(lastUnderlinedEnd > lastStyleEnd)
//                lastStyleEnd=lastUnderlinedEnd;
            underlined = false;
        }
        else if(s.getStyleType() == StyleType::Font){
            font = true;
            fontStart = vectorToEditorIndex(i);
        }
        else if(s.getStyleType() == StyleType::FontEnd && font == true){
            fontEnd = vectorToEditorIndex(i);
            currentCursor.setPosition(fontStart, QTextCursor::MoveAnchor);
            currentCursor.setPosition(fontEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontFamily(s.getFontName());
            mergeCurrentCharFormat(fmt);
            font = false;
        }
        else if(s.getStyleType() == StyleType::FontSize){
            fontsize = true;
            fontsizeStart = vectorToEditorIndex(i);
        }
        else if(s.getStyleType() == StyleType::FontSizeEnd && fontsize == true){
            fontsizeEnd = vectorToEditorIndex(i);
            currentCursor.setPosition(fontsizeStart, QTextCursor::MoveAnchor);
            currentCursor.setPosition(fontsizeEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setFontPointSize(s.getFontSize());
            mergeCurrentCharFormat(fmt);
            fontsize = false;
        }
        else if(s.getStyleType() == StyleType::Color){
            color = true;
            colorStart = vectorToEditorIndex(i);
        }
        else if(s.getStyleType() == StyleType::ColorEnd && color == true){
            colorEnd = vectorToEditorIndex(i);
            currentCursor.setPosition(colorStart, QTextCursor::MoveAnchor);
            currentCursor.setPosition(colorEnd, QTextCursor::KeepAnchor);
            this->setTextCursor(currentCursor);
            fmt.setForeground(QColor(s.getColor()));
            mergeCurrentCharFormat(fmt);
            color = false;
        }
    }
    this->setTextCursor(previousCursor);
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


void SharedEditor::printAll(){
    for (int i=0; i< _symbols.size(); i++) {
        qDebug() << QString::fromStdString(_symbols.at(i).toString());
    }
}








void SharedEditor::setEditorList(QString userlist){
    usernameToSiteId.clear();
    QStringList qlist = userlist.split(",");
    for(QString s : qlist){
        QString username = s.split(":").at(0);
        QString siteId = s.split(":").at(1);
        int site = siteId.toInt();
        usernameToSiteId.insert({username, site});
    }


    // HIDE CURSORS OF DISCONNECTED USERS
    if(siteIdToCursor.empty())
        return;
    QTextCursor cursor=textCursor();
    int originalPosition=cursor.position();
    QTextCharFormat fmt=QTextCharFormat();
    auto mapIt=siteIdToCursor.begin();
    while(mapIt!=siteIdToCursor.end()){
        std::map<QString,qint32>::iterator mapIt2;
        for(mapIt2=usernameToSiteId.begin();mapIt->first!=mapIt2->second&&mapIt2!=usernameToSiteId.end();++mapIt2);
        if(mapIt2!=usernameToSiteId.end()){
            mapIt++;
            continue;
        }
        //REMOVE BACKGROUND
        std::vector<Symbol>::iterator it;
        if((it=std::find(_symbols.begin(), _symbols.end(), mapIt->second)) !=_symbols.end()){
            int index = vectorToEditorIndex(std::distance(_symbols.begin(),it));
            cursor.setPosition(index, QTextCursor::MoveAnchor);
            cursor.setPosition(index+1, QTextCursor::KeepAnchor);
            this->setTextCursor(cursor);
            fmt.clearBackground();
            fmt.setBackground(QColor("#ffffff"));
            mergeCurrentCharFormat(fmt);
        }

        //REMOVE ENTRY
        siteIdToColor.erase(mapIt->first);
        mapIt = siteIdToCursor.erase(mapIt);

    }
    cursor.setPosition(originalPosition);
    setTextCursor(cursor);
}

std::map<QString,int> SharedEditor::getEditorList(){
    return usernameToSiteId;
}

void SharedEditor::showUserCursors(){
    if(siteIdToCursor.empty())
        return;
    QTextCursor cursor=textCursor();
    int originalPosition=cursor.position();
    QTextCharFormat fmt=QTextCharFormat();
    for(auto mapIt=siteIdToCursor.begin();mapIt!=siteIdToCursor.end();++mapIt){
        std::vector<Symbol>::iterator it;
        if((it=std::find(_symbols.begin(), _symbols.end(), mapIt->second)) !=_symbols.end()){
            int index = vectorToEditorIndex(std::distance(_symbols.begin(),it));
            cursor.setPosition(index, QTextCursor::MoveAnchor);
            cursor.setPosition(index+1, QTextCursor::KeepAnchor);
            this->setTextCursor(cursor);
            fmt.clearBackground();
            fmt.setBackground(siteIdToColor[mapIt->first]);
            mergeCurrentCharFormat(fmt);
        }
    }
    cursor.setPosition(originalPosition);
    setTextCursor(cursor);
}

void SharedEditor::hideUserCursors(){
    if(siteIdToCursor.empty())
        return;
    QTextCursor cursor=textCursor();
    int originalPosition=cursor.position();
    QTextCharFormat fmt=QTextCharFormat();
    for(auto mapIt=siteIdToCursor.begin();mapIt!=siteIdToCursor.end();++mapIt){
        std::vector<Symbol>::iterator it;
        if((it=std::find(_symbols.begin(), _symbols.end(), mapIt->second)) !=_symbols.end()){
            int index = vectorToEditorIndex(std::distance(_symbols.begin(),it));
            cursor.setPosition(index, QTextCursor::MoveAnchor);
            cursor.setPosition(index+1, QTextCursor::KeepAnchor);
            this->setTextCursor(cursor);
            fmt.clearBackground();
            fmt.setBackground(QColor("#ffffff"));
            mergeCurrentCharFormat(fmt);
        }
    }
    cursor.setPosition(originalPosition);
    setTextCursor(cursor);

}
