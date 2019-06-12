#ifndef QSHAREDEDITOR_H
#define QSHAREDEDITOR_H

#include <string>
#include <QTextEdit>
#include "../LSEQAllocator.h"
#include "../Symbol.h"
#include "../SymbolVector.h"
#include "../Message.h"
#include "../MessageQueue.h"



class QSharedEditor : public QTextEdit {
    Q_OBJECT
public:
    explicit QSharedEditor(QWidget *parent = nullptr);
    explicit QSharedEditor(const QString &text, QWidget *parent = nullptr);
    virtual ~QSharedEditor();


    void mergeCurrentCharFormat(const QTextCharFormat &modifier);
    void setCurrentCharFormat(const QTextCharFormat &format);
    void setAutoFormatting(AutoFormatting features);
    void setTabChangesFocus(bool b);
    inline void setDocumentTitle(const QString &title)
    { document()->setMetaInformation(QTextDocument::DocumentTitle, title); }
    void setLineWrapMode(LineWrapMode mode);
    void setLineWrapColumnOrWidth(int w);
    void setWordWrapMode(QTextOption::WrapMode policy);
    void setOverwriteMode(bool overwrite);
#if QT_DEPRECATED_SINCE(5, 10)
    QT_DEPRECATED void setTabStopWidth(int width);
#endif
    void setTabStopDistance(qreal distance);
    void setCursorWidth(int width);
    void setAcceptRichText(bool accept);
    void setExtraSelections(const QList<ExtraSelection> &selections);
    void moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

    virtual void keyPressEvent(QKeyEvent * e);

    void setExampleSiteId() {_siteId = 1; _counter=0;}
    void updatePreviousFields();
    int previousAnchor=0, previousPosition=0;
    QString previousSelection, previousLChar, previousRChar;
    int tempAnchor=0, tempPosition=0;
    QString tempSelection, tempLChar, tempRChar;
    bool fileLoading = true;

    void localSetStyle(int start, int end, StyleType style);
    void localUnsetStyle(int start, int end, StyleType style);

signals:
    void packetReady(QString);

private:
    int _siteId;
    int _counter;
    LSEQAllocator _lseq;
    std::vector<Symbol> _symbols;
    std::queue<Message> _mqIn, _mqOut;
    /**
     * Insert a char locally and send a Message to the server.
     *
     * @param index
     * @param value
     */
    void localInsert(int index, char value);
    void localInsertStyle(int index, StyleType style);
    /**
     * Erase a char locally and send a Message to the server
     * It needs to check if the erase provokes a style erasure.
     *
     * @param index
     */
    void localErase(int index);
    /**
     * Process the content of a message and execute its actions.
     *
     * @param m
     */
    void process(const Message &m);
    /**
     * Return the char sequence in the Editor from its internal representation.
     *
     * @return
     */
    std::string to_string();
    void setSiteId(int siteId);
    void eraseTwinTags();
    bool areTwinTags(Symbol a, Symbol b);
    int realIndex(int i);
};

#endif // QSHAREDEDITOR_H
