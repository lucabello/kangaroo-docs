#include "qsharededitor.h"
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

QSharedEditor::QSharedEditor(QWidget *parent) : QTextEdit(parent) {}

QSharedEditor::QSharedEditor(const QString &text, QWidget *parent) : QTextEdit(text, parent){}

QSharedEditor::~QSharedEditor(){}
/*
void QSharedEditor::mergeCurrentCharFormat(const QTextCharFormat &modifier){
    QTextEdit::mergeCurrentCharFormat(modifier);
    qDebug() << "mergeCurrentCharFormat";
}

void QSharedEditor::setCurrentCharFormat(const QTextCharFormat &format){
    QTextEdit::setCurrentCharFormat(format);
    qDebug() << "setCurrentCharFormat";
}

void QSharedEditor::setAutoFormatting(AutoFormatting features){
    QTextEdit::setAutoFormatting(features);
    qDebug() << "setAutoFormatting";
}

void QSharedEditor::setTabChangesFocus(bool b){
    QTextEdit::setTabChangesFocus(b);
    qDebug() << "setTabChangesFocus";
}

void QSharedEditor::setLineWrapMode(LineWrapMode mode){
    QTextEdit::setLineWrapMode(mode);
    qDebug() << "setLineWrapMode";
}

void QSharedEditor::setLineWrapColumnOrWidth(int w){
    QTextEdit::setLineWrapColumnOrWidth(w);
    qDebug() << "setLineWrapColumnOrWidth";
}

void QSharedEditor::setWordWrapMode(QTextOption::WrapMode policy){
    QTextEdit::setWordWrapMode(policy);
    qDebug() << "setWordWrapMode";
}

void QSharedEditor::setOverwriteMode(bool overwrite){
    QTextEdit::setOverwriteMode(overwrite);
    qDebug() << "setOverwriteMode";
}

void QSharedEditor::setTabStopWidth(int width){
    QTextEdit::setTabStopWidth(width);
    qDebug() << "setTabStopWidth";
}

void QSharedEditor::setTabStopDistance(qreal distance){
    QTextEdit::setTabStopDistance(distance);
    qDebug() << "setTabStopDistance";
}

void QSharedEditor::setCursorWidth(int width){
    QTextEdit::setCursorWidth(width);
    qDebug() << "setCursorWidth";
}

void QSharedEditor::setAcceptRichText(bool accept){
    QTextEdit::setAcceptRichText(accept);
    qDebug() << "setAcceptRichText";
}

void QSharedEditor::setExtraSelections(const QList<ExtraSelection> &selections){
    QTextEdit::setExtraSelections(selections);
    qDebug() << "setExtraSelections";
}

void QSharedEditor::moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode){
    QTextEdit::moveCursor(operation, mode);
    qDebug() << "moveCursor";
}*/

void QSharedEditor::updatePreviousFields(){
    this->previousAnchor = this->tempAnchor;
    this->previousPosition = this->tempPosition;
    this->previousSelection = this->tempSelection;
    this->previousLChar = this->tempLChar;
    this->previousRChar = this->tempRChar;
    this->tempAnchor = this->textCursor().anchor();
    this->tempPosition = this->textCursor().position();
    this->tempSelection = this->textCursor().selectedText();
    if(this->textCursor().position() > 0)
        this->tempLChar = this->toPlainText().at(this->textCursor().position()-1);
    else
        this->tempLChar = "";
    if(this->textCursor().position() < this->toPlainText().length())
        this->tempRChar = this->toPlainText().at(this->textCursor().position());
    else
        this->tempRChar = "";
}

bool isKeyPrintable(QKeyEvent * e){
    int k = e->key();
    int modifiers = e->modifiers();
    if(modifiers == Qt::ControlModifier){
        return false;
    }
    if(!(k == Qt::Key_Tab || k == Qt::Key_Backtab ||
            k == Qt::Key_Return	||
            k == Qt::Key_Enter ||
            (k >= 0x20 && k <= 0xdf) ||
            k == Qt::Key_division ||
            k == Qt::Key_ydiaeresis ||
            k == 0))
        return false;
    if(e->text().length() != 1)
        return false;
    return true;
}

bool isKeyPaste(QKeyEvent * e){
    int k = e->key();
    int modifiers = e->modifiers();
    if(modifiers == Qt::ControlModifier && k == Qt::Key_V)
        return true;
    return false;
}

void QSharedEditor::keyPressEvent(QKeyEvent * e){
    qDebug() << "KEY PRESS CAUGHT YAY";
    if(isKeyPrintable(e))
        qDebug() << "- Printable key: " << e->text() << " - key: " << e->key();
    else if(isKeyPaste(e))
        qDebug() << "- You pasted: " << QApplication::clipboard()->mimeData()->html();
    QTextEdit::keyPressEvent(e);
}

