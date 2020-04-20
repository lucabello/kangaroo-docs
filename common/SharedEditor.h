#ifndef SHAREDEDITOR_H
#define SHAREDEDITOR_H

#include <string>
#include <queue>
#include <QTextEdit>
#include "LSEQAllocator.h"
#include "../common/Symbol.h"
#include "../common/Message.h"
#include "../common/MessageQueue.h"


/**
 * The SharedEditor class represents the text box where text can be written.
 * It inherits from QTextEdit, integrating functions to handle collaborative
 * editing and removing some unwanted features.
 *
 * siteId is as a unique identifier assigned when connecting to the server.
 *   It is assigned after the login operation, and it is associated with
 *   the user account.
 * siteCounter is the relative counter of how many characters have been
 *   inserted from an editor in a certain file.
 * _mqIn and _mqOut are the two message queues for input and output.
 *
 * Keypresses are intercepted as are style additions (pressing the button of
 * bold or using a shortcut). Before these actions are executed normally,
 * their effects are replicated on the vector of symbols, so that it becomes a
 * parallel copy of the text which is suitable for collaborative editing, since
 * symbols are used instead of characters. Mantaining a precise correspondence
 * between this vector and the text is vital and key to the functioning of the
 * application. Every action happening on the text should be replicated on the
 * vector and communicated to the server with a proper Message.
 *
 * 3 threads should operate within this class:
 * - the main one handles the editor and the core of the application,
 *   inserting Symbols and pushing Messages into queues
 * - a thread only watches the output queue; whenever it is not empty,
 *   it wakes up and pops a message from the queue and it sends it to the
 *   server
 * - a thread only watches the input queue; whenever it is not empty,
 *   it wakes up and pops a message from the queue and it processes it
 *   applying changes to the symbol vector
 * EXCLUSIVE ACCESS TO STRUCTURES SHOULD BE PROVIDED BY LOCKING THE ENTIRETY OF
 * EVERY INSERT/ERASE/SET FUNCTION. THE symbol vector SHOULD BE ACCESSED
 * EXCLUSIVELY ALWAYS.
 */
class SharedEditor : public QTextEdit {
    Q_OBJECT
public:
    explicit SharedEditor(QWidget *parent = nullptr, int siteId = -1);
    //explicit SharedEditor(const QString &text, QWidget *parent = nullptr, int siteId = -1);
    virtual ~SharedEditor();
    void processPaste(const QMimeData *source);
    /**
     * Handles a keypress. It is used to replicate on the vector of symbols
     * every action triggered by a keypress.
     * For the application to work properly, EVERY SINGLE ACTION that is
     * performable with a keypress needs to be replicated.
     *
     * @param e the event that triggered the function
     */
    virtual void keyPressEvent(QKeyEvent * e);
    /**
     * Clears the text both in the editor and in the SymbolVector.
     * Whenever a new file is created, it is good to initialize it with
     * this function.
     *
     * Actually, after emptying the contents a few symbols are inserted:
     * - default style symbols (Paragraph, Arial, 12pt, black)
     * - a fake content symbol ('\0') to avoid default tags deletion due to
     *   them enclosing no content
     */
    virtual void clear();

    //Set fixed siteId for testing and debugging purposes
    //the real ones should be communicated from server!
    void setExampleSiteId();

    /**
     * Wrapper for localSetSimpleStyle and localSetComplexStyle.
     * Start and end are included.
     *
     * @param start index in the editor
     * @param end index in the editor
     * @param s
     */
    void localSetStyle(int start, int end, Symbol s);
    /**
     * Removes a style from a range of text. Start and end are included.
     *
     * @param start index in the editor
     * @param end index in the editor
     * @param s
     */
    void localUnsetStyle(int start, int end, Symbol s);
    /**
     * Sets the alignment of a paragraph by removing and adding a Paragraph
     * style symbol. If the paragraph already has the specified alignment,
     * it does nothing.
     *
     * @param position index in the editor
     * @param a
     */
    void localSetAlignment(int position, AlignmentType a);
    /**
     * Return the char sequence in the SharedEditor from its internal
     * representation.
     */
    QString to_string();
    int getSiteId();
    void setSiteId(int siteId);
    bool siteIdHasColor(qint32 siteId);
    QColor getSiteIdColor(qint32 siteId);

    void setEditorList(QString);

    std::map<QString,int> getEditorList();

    void hideUserCursors();
    void showUserCursors();

signals:
    /**
     * Emitted when a packet is ready to be sent.
     *
     * @param bytes
     * @param len
     */
    void packetReady(Message message);
    /**
     * Used to connect to server with a keypress, useful for debugging
     * purposes.
     */
    void connectToServer();

public slots:
    /**
     * Whenever a packet is received, take it to the processing function.
     *
     * @param m incoming message
     */
    void incomingPacket(Message m);

private:
    int _siteId;
    int _counter = 0;
    LSEQAllocator _lseq;
    //TODO: substitute with SymbolVector
    std::vector<Symbol> _symbols;
    //TODO: substitute with MessageQueue
    std::queue<Message> _mqIn, _mqOut;


    std::map<qint32,Symbol> siteIdToCursor;
    std::map<qint32,QColor> siteIdToColor;


//    QClipboard _clipboard;


    std::map<QString,qint32> usernameToSiteId;

    /**
     * Insert a character locally and send a Message to the server.
     *
     * @param index index in the vector
     * @param value
     */
    void localInsert(int index, QString value);
    /**
     * Insert a style locally and send a Message to the server.
     *
     * @param index index in the vector
     * @param value
     */

    AlignmentType findLastAlignment(int index);
    /**
     * Find last used alignment to set the next paragraph to it
     *
     * @param index index in the vector
     */

    void localInsertStyle(int index, Symbol styleSymbol);
    /**
     * Erase a char locally and send a Message to the server
     * It needs to check if the erase provokes a style erasure.
     *
     * @param index index in the vector
     */
    void localErase(int index);
    /**
     * Process the content of a message and execute its actions.
     *
     * @param m
     */
    void process(const Message &m);
    /**
     * Erase twin tags that have no content symbol between them. When a symbol
     * erasure happens, if that was the only content symbol between two twin
     * style symbols, they need to be erased.
     * If there are multiple tags enclosing no content symbol, this method
     * only removes the most internal pair. To remove all empty twin tags,
     * this it needs to be called multiple times.
     * The return value is how many twin tags were removed; there are no
     * more empty tags when it returns 0.
     *
     * Example of use:
     *   while(eraseTwinTags()!=0);
     *
     * At the moment, this method is called multiple times every time the
     * localErase method is called.
     *
     * @return number of empty tags removed
     */
    int eraseTwinTags();

    /**
     * propagateStyleToEditor
     *
     * @param index the position of the newly inserted style symbol
     */
    void propagateStyleToEditor(int index);

    void applyStylesToEditor();


    /**
     * In the Symbol vector there are both Content and Style symbols.
     * However, in the text box there are only characters.
     * This method converts an index from the editor view to the
     * vector view, basically skipping the styles that precede the
     * specified symbol.
     *
     * @param i index in the editor
     * @return index in the vector
     */
    int editorToVectorIndex(int i);
    /**
     * In the Symbol vector there are both Content and Style symbols.
     * However, in the text box there are only characters.
     * This method converts an index from the vector view to the
     * editor view, basically counting how many styles precede the
     * specified symbol and subtracting that number from the index.
     *
     * @param i index in the vector
     * @return index in the editor
     */
    int vectorToEditorIndex(int i);

    /**
     * Insert two style symbols enclosing the content between start and end.
     * The StyleType is embedded in the symbol s. A "Simple" style can be
     * bold, italic or underlined; any of them cannot be nested inside an
     * equal tag pair.
     * If the content already has that style, the method does nothing.
     *
     * @param start index in the editor
     * @param end index in the editor
     * @param s
     */
    void localSetSimpleStyle(int start, int end, Symbol s);
    /**
     * Removes a style from the content between start and end. If that "simple"
     * style is already applied on content including the symbols from start
     * to end, remove style only from the defined region.
     * If the content does not have the specified style, the method does
     * nothing.
     *
     * @param start index in the editor
     * @param end index in the editor
     * @param s
     */
    void localUnsetSimpleStyle(int start, int end, Symbol s);
    /**
     * As localSetComplexStyle but for "complex" styles, meaning they have
     * additional informations and so can be nested if those information
     * differ.
     * Example of this situation is applying the style "red color" to the
     * following example vector: <color: blue>AABBAA</color> .
     * If the content does not have the specified style, the method does
     * nothing.
     *
     * @param start index in the editor
     * @param end index in the editor
     * @param s
     */
    void localSetComplexStyle(int start, int end, Symbol s);
    /**
     * Erases the text in cursor selection. Whenever a symbol is inserted
     * or backspace/cancel is pressed with an active selection, all symbols
     * in the selection are erased by this method.
     *
     * @param e event that triggered the method
     */
    void eraseSelectedText(QKeyEvent* e);


    void moveUserCursor(int cursorStart,int cursorEnd,Message m);

    QColor randomColor();


    /**
     * Debbugging tool that prints all the symbols in the vector
     *
     * @param e event that triggered the method
     */
    void printAll();


    void avoidBackgroundPropagation(int editorIndexSym);

    int LastIndex();

    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);
};

#endif // SHAREDEDITOR_H
