//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_SYMBOL_H
#define KANGAROO_DOCS_SYMBOL_H

#include <vector>
#include <string>

enum SymbolType {CONTENT = 0, STYLE = 1};
enum StyleType {Paragraph, Bold, BoldEnd, Italic, ItalicEnd,
                Underlined, UnderlinedEnd, Color, ColorEnd};

/**
 * Char symbol represented as a CRDT (Common Replicated Data Type).
 */
class Symbol {
    char c;
    std::string tag;
    SymbolType type;
    StyleType style;
    int color;
    int alignment;
    int siteId;
    int siteCounter;
    std::vector<int> position;
public:
    Symbol();
    Symbol(char c, int site, int counter, std::vector<int> pos);
    Symbol(StyleType style, int site, int counter, std::vector<int> pos);
    bool operator<(const Symbol &other) const;
    std::vector<int> getPosition();
    char getContent();
    std::string getTag();
    StyleType getStyleType();
    int getColor();
    int getAlignment();
    bool isOpenTag();
    bool isCloseTag();
    bool isContent();
    bool isStyle();
    static StyleType getClosedStyle(StyleType s);
};


#endif //KANGAROO_DOCS_SYMBOL_H
