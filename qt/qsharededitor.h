#ifndef QSHAREDEDITOR_H
#define QSHAREDEDITOR_H

#include <QTextEdit>

class QSharedEditor : public QTextEdit {
public:
    explicit QSharedEditor(QWidget *parent = nullptr);
    explicit QSharedEditor(const QString &text, QWidget *parent = nullptr);
    virtual ~QSharedEditor();
/*
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
*/
    virtual void keyPressEvent(QKeyEvent * e);

    void updatePreviousFields();
    int previousAnchor=0, previousPosition=0;
    QString previousSelection, previousLChar, previousRChar;
    int tempAnchor=0, tempPosition=0;
    QString tempSelection, tempLChar, tempRChar;
    bool fileLoading = true;
};

#endif // QSHAREDEDITOR_H
