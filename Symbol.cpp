//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"

Symbol::Symbol() : c(0), siteId(-1), siteCounter(-1) {};

Symbol::Symbol(char c, int site, int counter, std::vector<int> pos) : c(c),
    siteId(site), siteCounter(counter), position(pos), type(SymbolType::CONTENT){};

Symbol::Symbol(StyleType style, int site, int counter, std::vector<int> pos) : style(style),
    siteId(site), siteCounter(counter), position(pos), type(SymbolType::STYLE){
    if(style == Paragraph)
        tag = "<p>";
    else if(style == Bold)
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
    else if(style == Color)
        tag = "<color>";
    else if(style == ColorEnd)
        tag = "</color>";
};

bool Symbol::operator<(const Symbol &other) const {
    int depth=0, p=-1, q=-1;
    int pSize=this->position.size(), qSize = other.position.size();
    if(pSize == 0 && qSize == 0) //both vectors are empty
        return false;
    do {
        p = (depth < pSize)? this->position.at(depth) : -1;
        q = (depth < qSize)? other.position.at(depth) : -1;
        depth++;
    } while (p == q && p != -1);
    if(p < q)
        return true;
    return false;
}

std::vector<int> Symbol::getPosition() {
    return position;
}

char Symbol::getContent() {
    return c;
}

std::string Symbol::getTag() {
    return tag;
}

StyleType Symbol::getStyleType(){
    return style;
}

int Symbol::getColor(){
    return color;
}

int Symbol::getAlignment(){
    return alignment;
}

bool Symbol::isOpenTag(){
    if(style == Bold || style == Italic || style == Underlined || style == Color)
        return true;
    return false;
}

bool Symbol::isCloseTag(){
    return !isOpenTag();
}

bool Symbol::isContent(){
    return (type == SymbolType::CONTENT);
}

bool Symbol::isStyle(){
    return (type == SymbolType::STYLE);
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
    return Paragraph; //should never get here
}
