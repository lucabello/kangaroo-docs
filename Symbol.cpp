//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"

//Constructors

Symbol::Symbol() : c(0), siteId(-1), siteCounter(-1) {};

Symbol::Symbol(wchar_t c, int site, int counter, std::vector<int> pos) : c(c),
    siteId(site), siteCounter(counter), position(pos), type(SymbolType::Content){};

Symbol::Symbol(StyleType style, int site, int counter, std::vector<int> pos) : style(style),
    siteId(site), siteCounter(counter), position(pos), type(SymbolType::Style){
    setProperTag();
};

Symbol::Symbol(StyleType style, AlignmentType alignment, int site, int counter, std::vector<int> pos) : style(style),
    alignment(alignment), siteId(site), siteCounter(counter), position(pos), type(SymbolType::Style){
    setProperTag();
};

Symbol::Symbol(StyleType style, std::string param, int site, int counter, std::vector<int> pos) : style(style),
    siteId(site), siteCounter(counter), position(pos), type(SymbolType::Style){
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

Symbol::Symbol(StyleType style, int fontsize, int site, int counter, std::vector<int> pos) : style(style),
    fontsize(fontsize), siteId(site), siteCounter(counter), position(pos), type(SymbolType::Style){
    setProperTag();
};

Symbol::Symbol(Symbol s, int site, int counter, std::vector<int> pos){
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

bool Symbol::isContent(){
    return (type == SymbolType::Content);
}

bool Symbol::isStyle(){
    return (type == SymbolType::Style);
}

std::vector<int> Symbol::getPosition() {
    return position;
}

//Getters

wchar_t Symbol::getContent() {
    return c;
}

StyleType Symbol::getStyleType(){
    return style;
}

std::string Symbol::getTag() {
    return tag;
}

AlignmentType Symbol::getAlignment(){
    return alignment;
}

std::string Symbol::getColor(){
    return color;
}

std::string Symbol::getFontName(){
    return fontname;
}

int Symbol::getFontSize(){
    return fontsize;
}

//Utilities

bool Symbol::isOpenTag(){
    if(style == Bold || style == Italic || style == Underlined || style == Color)
        return true;
    return false;
}

bool Symbol::isCloseTag(){
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

Symbol Symbol::getOpenStyle(Symbol s){
    Symbol result(s, s.siteId, s.siteCounter, s.position);
    result.style = getOpenStyle(s.style);
    result.setProperTag();
    return result;
}

Symbol Symbol::getClosedStyle(Symbol s){
    Symbol result(s, s.siteId, s.siteCounter, s.position);
    result.style = getClosedStyle(s.style);
    result.setProperTag();
    return result;
}

bool Symbol::isSameStyleAs(Symbol other){
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
    return false;
}

bool Symbol::isOpeningOf(Symbol other){
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

bool Symbol::areTwinTags(Symbol a, Symbol b){
    return (a.isOpeningOf(b) || b.isOpeningOf(a));
}

bool Symbol::areSimilarTags(Symbol a, Symbol b){
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
        tag = "<font-size:"+std::to_string(fontsize)+"pt>";
    else if(style == FontSizeEnd)
        tag = "</font-size:"+std::to_string(fontsize)+"pt>";

}

bool Symbol::isSimpleStyle(){
    if(style == Bold || style == BoldEnd)
        return true;
    if(style == Italic || style == ItalicEnd)
        return true;
    if(style == Underlined || style == UnderlinedEnd)
        return true;
    return false;
}

bool Symbol::isComplexStyle(){
    return !isSimpleStyle();
}

//Network

void pushIntToByteArray(int i, char *bytes, int *offset){
    bytes[0+*offset] = (i & 0xFF000000) >> 24;
    bytes[1+*offset] = (i & 0x00FF0000) >> 16;
    bytes[2+*offset] = (i & 0x0000FF00) >> 8;
    bytes[3+*offset] = (i & 0x000000FF);
    *offset = *offset + 4;
}

void pushWCharToByteArray(wchar_t c, char *bytes, int *offset){
    bytes[0+*offset] = (c & 0xFF00) >> 8;
    bytes[1+*offset] = (c & 0x00FF);
    *offset = *offset + 2;
}

int popIntFromByteArray(char *bytes, int *offset){
    int i = (bytes[0+*offset] << 24) | (bytes[1+*offset] << 16) | (bytes[2+*offset] << 8) | (bytes[3+*offset]);
    *offset = *offset + 4;
    return i;
}

wchar_t popWCharFromByteArray(char *bytes, int *offset){
    wchar_t c = (bytes[0+*offset] << 8) | (bytes[1+*offset]);
    *offset = *offset + 2;
    return c;
}

char* Symbol::serialize(Symbol s){
    char *bytes = new char[100]; //only for testing, REMEMBER TO DELETE CORRECTLY
    int offset=4;
    pushIntToByteArray(s.type, bytes, &offset);
    pushIntToByteArray(s.siteId, bytes, &offset);
    pushIntToByteArray(s.siteCounter, bytes, &offset);
    pushIntToByteArray(s.position.size(), bytes, &offset);
    for(int i=0; i<s.position.size(); i++)
        pushIntToByteArray(s.position.at(i), bytes, &offset);
    if(s.isContent())
        pushWCharToByteArray(s.c, bytes, &offset);
    else if(s.isStyle()){
        pushIntToByteArray(s.style, bytes, &offset);
        if(s.style == StyleType::Paragraph)
            pushIntToByteArray(s.alignment, bytes, &offset);
        else if(s.style == StyleType::Color){
            pushIntToByteArray(s.color.length(), bytes, &offset);
            for(int i=0; i<s.color.length(); i++)
                bytes[offset++] = s.color.at(i);
        }
        else if(s.style == StyleType::ColorEnd){
            pushIntToByteArray(s.color.length(), bytes, &offset);
            for(int i=0; i<s.color.length(); i++)
                bytes[offset++] = s.color.at(i);
        }
        else if(s.style == StyleType::Font){
            pushIntToByteArray(s.fontname.length(), bytes, &offset);
            for(int i=0; i<s.color.length(); i++)
                bytes[offset++] = s.fontname.at(i);
        }
        else if(s.style == StyleType::FontEnd){
            pushIntToByteArray(s.fontname.length(), bytes, &offset);
            for(int i=0; i<s.color.length(); i++)
                bytes[offset++] = s.fontname.at(i);
        }
        else if(s.style == StyleType::FontSize){
            pushIntToByteArray(s.fontsize, bytes, &offset);
        }
        else if(s.style == StyleType::FontSizeEnd){
            pushIntToByteArray(s.fontsize, bytes, &offset);
        }
    }
    int payloadLen = offset-4;
    int tmp = 0;
    pushIntToByteArray(payloadLen, bytes, &tmp);
    return bytes;
}

Symbol Symbol::unserialize(char *bytes){
    Symbol s;
    int offset=0;
    int payloadLen = popIntFromByteArray(bytes, &offset); //payload length
    s.type = (SymbolType)popIntFromByteArray(bytes, &offset);
    s.siteId = popIntFromByteArray(bytes, &offset);
    s.siteCounter = popIntFromByteArray(bytes, &offset);
    int posLen = popIntFromByteArray(bytes, &offset);
    for(int i=0; i<posLen; i++)
        s.position.push_back(popIntFromByteArray(bytes, &offset));
    if(s.isContent())
        s.c = popWCharFromByteArray(bytes, &offset);
    else if(s.isStyle()){
        s.style = (StyleType)popIntFromByteArray(bytes, &offset);
        if(s.style == StyleType::Paragraph)
            s.alignment = (AlignmentType)popIntFromByteArray(bytes, &offset);
        else if(s.style == StyleType::Color){
            int colLen = popIntFromByteArray(bytes, &offset);
            for(int i=0; i<colLen; i++)
                s.color.push_back(bytes[offset++]);
        }
        else if(s.style == StyleType::ColorEnd){
            int colLen = popIntFromByteArray(bytes, &offset);
            for(int i=0; i<colLen; i++)
                s.color.push_back(bytes[offset++]);
        }
        else if(s.style == StyleType::Font){
            int fontLen = popIntFromByteArray(bytes, &offset);
            for(int i=0; i<fontLen; i++)
                s.color.push_back(bytes[offset++]);
        }
        else if(s.style == StyleType::FontEnd){
            int fontLen = popIntFromByteArray(bytes, &offset);
            for(int i=0; i<fontLen; i++)
                s.color.push_back(bytes[offset++]);
        }
        else if(s.style == StyleType::FontSize){
            s.fontsize = popIntFromByteArray(bytes, &offset);
        }
        else if(s.style == StyleType::FontSizeEnd){
            s.fontsize = popIntFromByteArray(bytes, &offset);
        }
    }
    s.setProperTag();
    return s;
}
