//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_SYMBOL_H
#define KANGAROO_DOCS_SYMBOL_H

#include <vector>
#include <string>
#include <QDataStream>

enum SymbolType:qint32 {Content = 0, Style = 1};
enum StyleType:qint32 {Paragraph,
                Bold, BoldEnd,
                Italic, ItalicEnd,
                Underlined, UnderlinedEnd,
                Color, ColorEnd,
                Font, FontEnd,
                FontSize, FontSizeEnd};
enum AlignmentType:qint32 {AlignLeft, AlignCenter, AlignRight, AlignJustified};

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
 * ALWAYS CHECK THE TYPE BEFORE READING AN ATTRIBUTE.
 */
class Symbol {
    SymbolType type;
    qint32 siteId;
    qint32 siteCounter;
    std::vector<qint32> position;

    wchar_t c;
    StyleType style;
    /**
     * Tag is used to represent a human-readable form of a style symbol.
     * It is only used to check for correctness in debug prints.
     */
    QString tag;
    //Alignment is specific to a Paragraph style
    AlignmentType alignment;
    QString color;
    QString fontname;
    int fontsize;

public:
    //Default constructor
    Symbol();
    //Copy constructor
    Symbol(const Symbol& old);
    //TODO: Assignment operator
    //TODO: Destructor
    //Content constructor
    Symbol(wchar_t c, int site, int counter, std::vector<int> pos);
    //Style constructor for Bold, Italic, Underlined
    Symbol(StyleType style, int site, int counter, std::vector<int> pos);
    //Style constructor for Paragraph
    Symbol(StyleType style, AlignmentType alignment, int site, int counter, std::vector<int> pos);
    //Style constructor for Color or Font
    Symbol(StyleType style, QString param, int site, int counter, std::vector<int> pos);
    //Style constructor for FontSize
    Symbol(StyleType style, int fontsize, int site, int counter, std::vector<int> pos);
    //Style constructor for copy
    Symbol(Symbol& s, int site, int counter, std::vector<int> pos);

    //general methods
    bool operator<(const Symbol &other) const;
    bool operator==(const Symbol &other) const;
    bool isContent() const;
    bool isStyle() const;
    bool isFake() const;
    std::vector<int> getPosition();
    /**
     * Returns a human-readable string with all symbol relevant properties,
     * to check for correctness when debugging the application.
     *
     * @return a readable digest of the symbol
     */
    std::string toString();

    //getters
    SymbolType getType() const;
    qint32 getSiteId() const;
    qint32 getSiteCounter() const;
    std::vector<qint32> getPosition() const;
    wchar_t getContent() const;
    StyleType getStyleType() const;
    QString getTag() const;
    AlignmentType getAlignment() const;
    QString getColor() const;
    QString getFontName() const;
    qint32 getFontSize() const;

    QString getPlaintext() const;

    //utilites
    /**
     * Checks if a tag is the opening of a style. Returns false for a
     * paragraph.
     */
    bool isOpenTag();
    /**
     * Checks if a tag is the closure of a style. Returns false for a
     * paragraph.
     */
    bool isCloseTag();
    /**
     * Returns the open StyleType matching to the closed one passed as
     * parameter.
     * s must be a closed StyleType and not a paragraph.
     * StyleType Paragraph is returned in case of error.
     *
     * @param s a closed style
     * @return the matching open style
     */
    static StyleType getOpenStyle(StyleType s);
    /**
     * Returns the closed StyleType matching to the open one passed as
     * parameter.
     * s must be an open StyleType and not a paragraph.
     * StyleType Paragraph is returned in case of error.
     *
     * @param s an open style
     * @return the matching closed style
     */
    static StyleType getClosedStyle(StyleType s);
    /**
     * Overload of getOpenStyle building an open version of the symbol
     * passed as parameter.
     *
     * @param s a closed symbol
     * @return a matching open symbol
     */
    static Symbol getOpenStyle(Symbol& s);
    /**
     * Overload of getOpenStyle building a closed version of the symbol
     * passed as parameter.
     *
     * @param s an open symbol
     * @return a matching closed symbol
     */
    static Symbol getClosedStyle(Symbol& s);
    /**
     * Checks if two Symbols are equal style symbols.
     *
     * @param other the symbol to compare to
     * @return true if both symbols represent the same style
     */
    bool isSameStyleAs(Symbol& other);
    /**
     * Checks whether a symbol is the opening style of another one.
     *
     * @param other the symbol to compare to
     */
    bool isOpeningOf(Symbol& other);

    /**
     * Checks whether a symbol is the closing style of another one.
     *
     * @param other the symbol to compare to
     */
    bool isClosingOf(Symbol& other);

    /**
     * Checks if two symbols are twin tags, which means that one is the
     * opening of the other one
     *
     * @param a a style symbol
     * @param b a style symbol
     * @return true if symbols are twin tags
     */

    static bool areTwinTags(Symbol& a, Symbol& b);
    /**
     * Checks if two symbols are similar tags, which means that the StyleType
     * is the same or the two have twin StyleTypes.
     * Does not look at style contents as specific color or font name.
     *
     * @param a a style symbol
     * @param b a style symbol
     * @return true if symbols are similar tags
     */
    static bool areSimilarTags(Symbol& a, Symbol& b);
    /**
     * Sets the proper tag label to the symbol looking at the style type
     * and content.
     */
    void setProperTag();
    /**
     * Checks if the symbol has a "Simple" style, that means it has no
     * additional parameters to enrich it.
     *
     * @return true if style is of type bold, italic or underlined
     */
    bool isSimpleStyle();
    /**
     * Checks if the symbols has a "Complex" style, that means it has
     * additional parameters to enrich it.
     *
     * @return true if style is of type color, font, fontsize or paragraph
     */
    bool isComplexStyle();

    friend QDataStream &operator<<(QDataStream &out, const Symbol &item);
    friend QDataStream &operator>>(QDataStream &in, Symbol &item);

};


#endif //KANGAROO_DOCS_SYMBOL_H
