//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_SYMBOL_H
#define KANGAROO_DOCS_SYMBOL_H

#include <vector>
#include <string>

enum SymbolType {Content = 0, Style = 1};
enum StyleType {Paragraph,
                Bold, BoldEnd,
                Italic, ItalicEnd,
                Underlined, UnderlinedEnd,
                Color, ColorEnd,
                Font, FontEnd,
                FontSize, FontSizeEnd};
enum AlignmentType {AlignLeft, AlignCenter, AlignRight};

/**
 * Symbol represented as a CRDT (Common Replicated Data Type).
 *
 * A symbol can represent a unicode character (SymbolType::Content)
 * or a meta-character for styling purposes (SymbolType::Style).
 *
 * Implemented meta-characters and their attributes are the following:
 * - Paragraph
 * --- Starts a new paragraph. It is always present at the start of the document
 * --- and it is automatically after a '\n'. Whenever a character is inserted at
 * --- the start of a paragraph, it inherits the styles of the following word.
 * --- Attributes: AlignmentType alignment
 * - Bold
 * - Italic
 * - Underlined
 * - Color
 * --- Attributes: std::string color (string representing hex code)
 * - Font
 * --- Defines a font (Arial, etc).
 * --- Attributes: std::string fontname
 * - FontSize
 * --- Defines the size of the text.
 * --- Attributes: int fontsize
 *
 * ALWAYS CHECK BEFORE READING AN ATTRIBUTE.
 */
class Symbol {
    SymbolType type;
    int siteId;
    int siteCounter;
    std::vector<int> position;

    wchar_t c;
    StyleType style;
    std::string tag;
    AlignmentType alignment;
    std::string color;
    std::string fontname;
    int fontsize;

public:
    Symbol();
    //Content constructor
    Symbol(wchar_t c, int site, int counter, std::vector<int> pos);
    //Style constructor for Bold, Italic, Underlined
    Symbol(StyleType style, int site, int counter, std::vector<int> pos);
    //Style constructor for Paragraph
    Symbol(StyleType style, AlignmentType alignment, int site, int counter, std::vector<int> pos);
    //Style constructor for Color or Font
    Symbol(StyleType style, std::string param, int site, int counter, std::vector<int> pos);
    //Style constructor for FontSize
    Symbol(StyleType style, int fontsize, int site, int counter, std::vector<int> pos);
    //Style constructor for copy
    Symbol(Symbol s, int site, int counter, std::vector<int> pos);

    //general methods
    bool operator<(const Symbol &other) const;
    bool isContent();
    bool isStyle();
    std::vector<int> getPosition();

    //getters
    wchar_t getContent();
    StyleType getStyleType();
    std::string getTag();
    AlignmentType getAlignment();
    std::string getColor();
    std::string getFontName();
    int getFontSize();

    //utilites
    bool isOpenTag();
    bool isCloseTag();
    static StyleType getClosedStyle(StyleType s);
    static Symbol getClosedStyle(Symbol s);
    bool isSameStyleAs(Symbol other);
    bool isOpeningOf(Symbol other);
    static bool areTwinTags(Symbol a, Symbol b);
    static bool areSimilarTags(Symbol a, Symbol b);
    void setProperTag();

    //network
    //quick and dirty: this serialization depends on the endiannes of the machine
    //CHANGE IT TO BE INDEPENDENT FROM MACHINE'S ENDIANNES !!!
    static char* serialize(Symbol s);
    static Symbol unserialize(char* bytes);
};


#endif //KANGAROO_DOCS_SYMBOL_H
