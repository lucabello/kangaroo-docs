//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"
#include <QDebug>
//Constructors

Symbol::Symbol():siteId(-1),siteCounter(-1),c(""){};

Symbol::Symbol(const Symbol& old) : type(old.type),siteId(old.siteId), siteCounter(old.siteCounter), position(old.position){
    if(old.type == SymbolType::Content){
        c = old.c;
    } else if (old.type == SymbolType::Style) {
        style = old.style;
        tag = old.tag;
        if(style == StyleType::Paragraph)
            alignment = old.alignment;
        else if(style == StyleType::Color || style == StyleType::ColorEnd)
            color = old.color;
        else if(style == StyleType::Font || style == StyleType::FontEnd)
            fontname = old.fontname;
        else if(style == StyleType::FontSize || style == StyleType::FontSizeEnd)
            fontsize = old.fontsize;
    }
}

Symbol::Symbol(qint32 siteId,qint32 siteCounter,std::vector<qint32> pos,QString c) :
    type(SymbolType::Content),siteId(siteId),siteCounter(siteCounter),position(pos),c(c){};

Symbol::Symbol(qint32 siteId,qint32 siteCounter,std::vector<qint32> pos,StyleType style) : type(SymbolType::Style),
    siteId(siteId), siteCounter(siteCounter), position(pos),style(style){
    setProperTag();
};

Symbol::Symbol(qint32 siteId,qint32 siteCounter,std::vector<qint32> pos,StyleType style, AlignmentType alignment) :
    type(SymbolType::Style),siteId(siteId),siteCounter(siteCounter),position(pos),style(style),alignment(alignment){
    setProperTag();
};

Symbol::Symbol(qint32 siteId,qint32 siteCounter,std::vector<qint32> pos,StyleType style, QString param):
    type(SymbolType::Style),siteId(siteId), siteCounter(siteCounter), position(pos),style(style){
    if(style == StyleType::Color)
        color = param;
    else if(style == StyleType::ColorEnd)
        color = param;
    else if(style == StyleType::Font)
        fontname = param;
    else if(style == StyleType::FontEnd)
        fontname = param;
    setProperTag();
};

Symbol::Symbol(qint32 siteId,qint32 siteCounter,std::vector<qint32> pos,StyleType style, int fontsize):
    type(SymbolType::Style),siteId(siteId), siteCounter(siteCounter), position(pos),style(style),fontsize(fontsize){
    setProperTag();
};

Symbol::Symbol(Symbol& s, int site, int counter, std::vector<int> pos){
    this->type = s.type;
    this->siteId = site;
    this->siteCounter = counter;
    this->position = pos;

    this->style = s.style;
    this->tag = s.tag;
    this->alignment = s.alignment;
    this->color = s.color;
    this->fontname = s.fontname;
    this->fontsize = s.fontsize;
}

//General methods

//Compare only even elements first; if equal, also use siteIds to establish relative order
bool Symbol::operator<(const Symbol &other) const {
    int p=-1, q=-1;
    __SIZE_TYPE__ depth=0,pSize=0, qSize=0;
    pSize=this->position.size();
    qSize = other.position.size();
    if(pSize == 0 && qSize == 0) //both vectors are empty
        return false;
    //compare only even elements
    do {
        p = (depth < pSize)? this->position.at(depth) : -1;
        q = (depth < qSize)? other.position.at(depth) : -1;
        depth += 2; //only check even elements; odds are siteIds
    } while (p == q && p != -1);
    if(p < q)
        return true;
    if(p > q)
        return false;
    //p == q and so they are both equal to -1
    do {
        p = (depth < pSize)? this->position.at(depth) : -1;
        q = (depth < qSize)? other.position.at(depth) : -1;
        depth++; //check all elements
    } while (p == q && p != -1);
    if(p < q)
        return true;
    return false;
}

bool Symbol::operator==(const Symbol &other) const {
    bool result = ( (!(*this < other)) && (!(other < *this)) );
    return result;
}

bool Symbol::isContent() const{
    return (type == SymbolType::Content);
}

bool Symbol::isStyle() const{
    return (type == SymbolType::Style);
}

bool Symbol::isFake() const{
    return ((siteId==0)&&(siteCounter==0));
}

std::vector<int> Symbol::getPosition() {
    return position;
}

std::string Symbol::toString(){
    std::string result = "Symbol("+std::to_string(siteId)+","+std::to_string(siteCounter)+"): ";
    if(this->type == SymbolType::Style)
        result += this->tag.toStdString();
    else if(this->type == SymbolType::Content)
        result += this->c.toStdString();
    result += " Position: ";
    for(int a : this->position)
        result += std::to_string(a) + ",";
    return result;
}

//Getters

SymbolType Symbol::getType() const{
    return type;
}

qint32 Symbol::getSiteId() const{
    return siteId;
}

qint32 Symbol::getSiteCounter() const{
    return siteCounter;
}

std::vector<qint32> Symbol::getPosition() const{
    return position;
}

QString Symbol::getContent() const{
    return c;
}

StyleType Symbol::getStyleType() const{
    return style;
}

QString Symbol::getTag() const{
    return tag;
}

AlignmentType Symbol::getAlignment() const{
    return alignment;
}

QString Symbol::getColor() const{
    return color;
}

QString Symbol::getFontName() const{
    return fontname;
}

qint32 Symbol::getFontSize() const{
    return fontsize;
}



/*QString Symbol::getPlaintext() const{
    char16_t ch = c;
    QString result = QString::fromUtf16(&ch, 1);
    return result;
}*/

//Utilities

bool Symbol::isOpenTag(){
    if(!this->isStyle())
        return false;
    if(style == StyleType::Bold || style == StyleType::Italic || style == StyleType::Underlined
            || style == StyleType::Color || style == StyleType::Font || StyleType::FontSize)
        return true;
    return false;
}

bool Symbol::isCloseTag(){
    if(style == StyleType::Paragraph)
        return false;
    return !isOpenTag();
}

StyleType Symbol::getOpenStyle(StyleType s){
    if(s == StyleType::BoldEnd)
        return Bold;
    if(s == StyleType::ItalicEnd)
        return Italic;
    if(s == StyleType::UnderlinedEnd)
        return Underlined;
    if(s == StyleType::ColorEnd)
        return Color;
    if(s == StyleType::FontEnd)
        return Font;
    if(s == StyleType::FontSizeEnd)
        return FontSize;
    return Paragraph; //should never get here
}

StyleType Symbol::getClosedStyle(StyleType s){
    if(s == StyleType::Bold)
        return BoldEnd;
    if(s == StyleType::Italic)
        return ItalicEnd;
    if(s == StyleType::Underlined)
        return UnderlinedEnd;
    if(s == StyleType::Color)
        return ColorEnd;
    if(s == StyleType::Font)
        return FontEnd;
    if(s == StyleType::FontSize)
        return FontSizeEnd;
    return Paragraph; //should never get here
}

Symbol Symbol::getOpenStyle(Symbol& s){
    Symbol result(s, s.siteId, s.siteCounter, s.position);
    qDebug() << s.siteId << ", " << s.siteCounter << ", " << s.position << ", "<< s.style << ", " << s.color;
    result.style = getOpenStyle(s.style);
    result.setProperTag();
    return result;
}

Symbol Symbol::getClosedStyle(Symbol& s){
    Symbol result(s, s.siteId, s.siteCounter, s.position);
    qDebug() << s.siteId << ", " << s.siteCounter << ", " << s.position << ", "<< s.style << ", " << s.color;
    result.style = getClosedStyle(s.style);
    result.setProperTag();
    return result;
}

bool Symbol::isSameStyleAs(Symbol& other){
    if(!this->isStyle() || !other.isStyle())
        return false;
    if(this->style == StyleType::Bold && other.style == StyleType::Bold)
        return true;
    if(this->style == StyleType::Italic && other.style == StyleType::Italic)
        return true;
    if(this->style == StyleType::Underlined && other.style == StyleType:: Underlined)
        return true;
    if(this->style == StyleType::Color && other.style == StyleType::Color)
        if(this->color == other.color)
            return true;
    if(this->style == StyleType::Font && other.style == StyleType::Font)
        if(this->fontname == other.fontname)
            return true;
    if(this->style == StyleType::FontSize && other.style == StyleType::FontSize)
        if(this->fontsize == other.fontsize)
            return true;
    //The following is not relevant since it is never used
    if(this->style == StyleType::BoldEnd && other.style == StyleType::BoldEnd)
        return true;
    if(this->style == StyleType::ItalicEnd && other.style == StyleType::ItalicEnd)
        return true;
    if(this->style == StyleType::UnderlinedEnd && other.style == StyleType:: UnderlinedEnd)
        return true;
    if(this->style == StyleType::ColorEnd && other.style == StyleType::ColorEnd)
        if(this->color == other.color)
            return true;
    if(this->style == StyleType::FontEnd && other.style == StyleType::FontEnd)
        if(this->fontname == other.fontname)
            return true;
    if(this->style == StyleType::FontSizeEnd && other.style == StyleType::FontSizeEnd)
        if(this->fontsize == other.fontsize)
            return true;
    return false;
}

bool Symbol::isOpeningOf(Symbol& other){
    if(!this->isStyle() || !other.isStyle())
        return false;
    if(this->style == StyleType::Bold && other.style == StyleType::BoldEnd)
        return true;
    if(this->style == StyleType::Italic && other.style == StyleType::ItalicEnd)
        return true;
    if(this->style == StyleType::Underlined && other.style == StyleType:: UnderlinedEnd)
        return true;
    if(this->style == StyleType::Color && other.style == StyleType::ColorEnd)
        if(this->color == other.color)
            return true;
    if(this->style == StyleType::Font && other.style == StyleType::FontEnd)
        if(this->fontname == other.fontname)
            return true;
    if(this->style == StyleType::FontSize && other.style == StyleType::FontSizeEnd)
        if(this->fontsize == other.fontsize)
            return true;
    return false;
}

bool Symbol::isClosingOf(Symbol& other){
    if(!this->isStyle() || !other.isStyle())
        return false;
    if(this->style == StyleType::BoldEnd && other.style == StyleType::Bold)
        return true;
    if(this->style == StyleType::ItalicEnd && other.style == StyleType::Italic)
        return true;
    if(this->style == StyleType::UnderlinedEnd && other.style == StyleType:: Underlined)
        return true;
    if(this->style == StyleType::ColorEnd && other.style == StyleType::Color)
        if(this->color == other.color)
            return true;
    if(this->style == StyleType::FontEnd && other.style == StyleType::Font)
        if(this->fontname == other.fontname)
            return true;
    if(this->style == StyleType::FontSizeEnd && other.style == StyleType::FontSize)
        if(this->fontsize == other.fontsize)
            return true;
    return false;
}

bool Symbol::isClosingTag(){
    if(!this->isStyle())
        return false;
    if(this->style==StyleType::BoldEnd || this->style==StyleType::ItalicEnd || this->style==StyleType::UnderlinedEnd
            || this->style==StyleType::ColorEnd || this->style==StyleType::FontEnd || this->style==StyleType::FontSizeEnd)
        return true;
    else
        return false;
}

bool Symbol::areTwinTags(Symbol& a, Symbol& b){
    return (a.isOpeningOf(b) || b.isOpeningOf(a));
}

bool Symbol::areSimilarTags(Symbol& a, Symbol& b){
    if(!a.isStyle() || !b.isStyle())
        return false;
    if(a.style == b.style)
        return true;
    if((a.style == StyleType::Bold && b.style == StyleType::BoldEnd) ||
            (b.style == StyleType::Bold && a.style == StyleType::BoldEnd))
        return true;
    if((a.style == StyleType::Italic && b.style == StyleType::ItalicEnd) ||
            (b.style == StyleType::Italic && a.style == StyleType::ItalicEnd))
        return true;
    if((a.style == StyleType::Underlined && b.style == StyleType:: UnderlinedEnd) ||
            (b.style == StyleType::Underlined && a.style == StyleType:: UnderlinedEnd))
        return true;
    if((a.style == StyleType::Color && b.style == StyleType::ColorEnd) ||
            (b.style == StyleType::Color && a.style == StyleType::ColorEnd))
        return true;
    if((a.style == StyleType::Font && b.style == StyleType::FontEnd) ||
            (b.style == StyleType::Font && a.style == StyleType::FontEnd))
        return true;
    if((a.style == StyleType::FontSize && b.style == StyleType::FontSizeEnd) ||
            (b.style == StyleType::FontSize && a.style == StyleType::FontSizeEnd))
        return true;
    return false;
}

void Symbol::setProperTag(){
    if(style == Bold)
        tag = "<b>";
    else if(style == BoldEnd)
        tag = "</b>";
    else if(style == Italic)
        tag = "<i>";
    else if(style == ItalicEnd)
        tag = "</i>";
    else if(style == Underlined)
        tag = "<u>";
    else if(style == UnderlinedEnd)
        tag = "</u>";
    else if(style == Paragraph){
        if(alignment == AlignLeft)
            tag = "<p align:left/>";
        else if(alignment == AlignCenter)
            tag = "<p align:center/>";
        else if(alignment == AlignRight)
            tag = "<p align:right/>";
    }
    else if(style == Color)
        tag = "<color:"+color+">";
    else if(style == ColorEnd)
        tag = "</color:"+color+">";
    else if(style == Font)
        tag = "<font:"+fontname+">";
    else if(style == FontEnd)
        tag = "</font:"+fontname+">";
    else if(style == FontSize)
        tag = "<font-size:"+QString::fromStdString(std::to_string(fontsize))+"pt>";
    else if(style == FontSizeEnd)
        tag = "</font-size:"+QString::fromStdString(std::to_string(fontsize))+"pt>";

}

bool Symbol::isSimpleStyle(){
    if(type != SymbolType::Style)
        return false;
    if(style == Bold || style == BoldEnd)
        return true;
    if(style == Italic || style == ItalicEnd)
        return true;
    if(style == Underlined || style == UnderlinedEnd)
        return true;
    return false;
}

bool Symbol::isComplexStyle(){
    if(!this->isStyle())
        return false;
    return !isSimpleStyle();
}

QDataStream &operator<<(QDataStream &out, const Symbol &item)
{
    out << item.getType()
        << item.getSiteId()
        << item.getSiteCounter()
        << static_cast<qint32>(item.getPosition().size());
    for(qint32 i:item.getPosition())
        out << i;
    if(item.isContent()){
        out << item.getContent();
    }else if(item.isStyle()){
        out << item.getStyleType();
        switch(item.getStyleType()){
            case StyleType::Paragraph:
                out << item.getAlignment();
                break;
            case StyleType::Color:
            case StyleType::ColorEnd:
                out << item.getColor();
                break;
            case StyleType::Font:
            case StyleType::FontEnd:
                out << item.getFontName();
                break;
            case StyleType::FontSize:
            case StyleType::FontSizeEnd:
                out << item.getFontSize();
                break;
            default:
                break;
        }

    }
    return out;
    /* int payloadLen = offset-4;
    int tmp = 0;
    pushIntToByteArray(payloadLen, bytes, &tmp);*/
}

QDataStream &operator>>(QDataStream &in, Symbol &item){
    qint32 type;
    qint32 siteId;
    qint32 siteCounter;
    std::vector<qint32> position;
    QString c;
    qint32 style;
    QString tag;
    qint32 alignment;
    QString color;
    QString fontname;
    qint32 fontsize;

    in >> type;
    in >> siteId;
    in >> siteCounter;
    qint32 positionSize;
    in >> positionSize;
    for(qint32 i=0;i<positionSize;i++){
        qint32 positionElement;
        in >> positionElement;
        position.push_back(positionElement);
    }
    if(type == SymbolType::Content){
        in >> c;
        item = Symbol{siteId, siteCounter, position,c};
        qDebug()<<QString::fromStdString(item.toString());
    }else if(type == SymbolType::Style){
        in >> style;
        switch (style) {
            case StyleType::Paragraph:
                in >> alignment;
                item = Symbol{siteId,siteCounter,position,static_cast<StyleType>(style), static_cast<AlignmentType>(alignment)};
                break;
            case StyleType::Bold:
            case StyleType::BoldEnd:
            case StyleType::Italic:
            case StyleType::ItalicEnd:
            case StyleType::Underlined:
            case StyleType::UnderlinedEnd:
                item = Symbol{siteId,siteCounter,position,static_cast<StyleType>(style)};
                break;
            case StyleType::Color:
            case StyleType::ColorEnd:
                in >> color;
                item = Symbol{siteId,siteCounter,position,static_cast<StyleType>(style), color};
                break;
            case StyleType::Font:
            case StyleType::FontEnd:
                in >> fontname;
                item = Symbol{siteId,siteCounter,position,static_cast<StyleType>(style), fontname};
                break;
            case StyleType::FontSize:
            case StyleType::FontSizeEnd:
                in >> fontsize;
                item = Symbol{siteId,siteCounter,position,static_cast<StyleType>(style), fontsize};
                break;
            default:
                break;
        }
    }
    return in;

}
