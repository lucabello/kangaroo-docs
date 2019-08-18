/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include "ClientSocket.h"
#include "../common/SharedEditor.h"

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;
class QWindow;
QT_END_NAMESPACE

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);

    bool load(const QString &f);

signals:
    void newFileListWindow(ClientSocket*);
    void showLoginWindow();

public slots:
    void fileNew();
    void logout();
    void showTextEdit(ClientSocket*);
    void siteIdReceived(int);
    void changeFileName(QString);
    void hideWindow();
    void setEditorList(QString);

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    //Already present functions
    /**
     * Dialog window to open a file.
     */
    void fileOpen();
    /**
     * Saves a file to disk.
     * @return true for success
     */
    bool fileSave();
    /**
     * Saves a file as ... to disk.
     * @return true for success
     */
    bool fileSaveAs();
    /**
     * Prints the document.
     */
    void filePrint();
    /**
     * Previews the printing of the document connecting the slot
     * "printPreview".
     */
    void filePrintPreview();
    /**
     * Prints the document to PDF.
     */
    void filePrintPdf();

    //Modified functions
    /**
     * Allows registered user to share URI by email.
     */
    void shareURI();
    /**
     * Sets selected text as bold, both on SymbolVector and on editor.
     */
    void textBold();
    /**
     * Sets selected text as underlined, both on SymbolVector and on editor.
     */
    void textUnderline();
    /**
     * Sets selected text as italic, both on SymbolVector and on editor.
     */
    void textItalic();
    /**
     * Changes the font of the selected text, both on SymbolVector and
     * on editor.
     */
    void textFamily(const QString &f);
    /**
     * Changes the size of the selected text, both on SymbolVector and
     * on editor.
     */
    void textSize(const QString &p);
    //void textStyle(int styleIndex);
    /**
     * Changes the color of the selected text, both on SymbolVector and
     * on editor.
     */
    void textColor();
    /**
     * Changes the alignment of the selected text, both on SymbolVector and
     * on editor.
     */
    void textAlign(QAction *a);

    //Already present functions
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter *);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void showConnectedUsers();
    QString generateURI();

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    //QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    SharedEditor *textEdit;
    ClientSocket *tcpSocket;
};

#endif // TEXTEDIT_H
