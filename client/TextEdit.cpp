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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QGroupBox>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QDesktopWidget>
#include <QInputDialog>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#include "ui_shareuri.h"
#include "ui_accountInfo.h"
#include "ui_editNick.h"
#include "TextEdit.h"
#include "ClientSocket.h"
//#define QT_NO_CLIPBOARD

#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/win";
#else
const QString rsrcPath = ":/images/win";
#endif

TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    //textEdit = new QTextEdit(this);
    textEdit = new SharedEditor(this);
    textEdit->setExampleSiteId();
//    resize(this->width(), this->height());
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
//    const QRect screen = QDesktopWidget::availableGeometry(this);
    resize(availableGeometry.width()/2, availableGeometry.height()/2);

    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
//    connect(textEdit, SIGNAL(&QTextEdit::copyAvailable), this, SLOT(&TextEdit::copy));
//    connect(textEdit, &QTextEdit::paste, this, &TextEdit::processPaste);

    setCentralWidget(textEdit);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();

    QMenu *accountMenu = menuBar()->addMenu(tr("User Account"));
    accountMenu->addAction(tr("Info"), this, &TextEdit::accountInfo);
    accountMenu->addAction(tr("Edit nickname"), this, &TextEdit::editNick);

    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(tr("About"), this, &TextEdit::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);


    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    //connect(textEdit, &SharedEditor::setEditorList,
    //        this, &TextEdit::setEditorList);
    //connect(textEdit->document(), &QTextDocument::modificationChanged,
    //        actionSave, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    //connect(textEdit->document(), &QTextDocument::undoAvailable,
    //        actionUndo, &QAction::setEnabled);
    //connect(textEdit->document(), &QTextDocument::redoAvailable,
    //        actionRedo, &QAction::setEnabled);

    setWindowModified(textEdit->document()->isModified());
    //actionSave->setEnabled(textEdit->document()->isModified());
    //actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    //actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
//    actionCut->setEnabled(false);
//    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);
#endif

    textEdit->setFocus();
    setCurrentFileName(QString());

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif

  name = "";
  nick = "";
  siteID = "";
}

void TextEdit::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}

void TextEdit::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    const QIcon logoutIcon = QIcon::fromTheme("document-logout", QIcon(rsrcPath + "/logout.png"));
    QAction *a = menu->addAction(logoutIcon,  tr("&Logout"), this, &TextEdit::logout);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_L);
    tb->addAction(a);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    a = menu->addAction(openIcon, tr("&Open..."), this, &TextEdit::fileOpen);
    a->setShortcut(QKeySequence::Open);
    tb->addAction(a);

    menu->addSeparator();


    const QIcon infoIcon = QIcon::fromTheme("document-info", QIcon(rsrcPath + "/connectededitors.png"));
    a = menu->addAction(infoIcon, tr("&Show connected users..."), this, &TextEdit::showConnectedUsers);
    a->setShortcut(Qt::CTRL + Qt::Key_H);
    tb->addAction(a);
/*
    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    actionSave = menu->addAction(saveIcon, tr("&Save"), this, &TextEdit::fileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    tb->addAction(actionSave);

    a = menu->addAction(tr("Save &As..."), this, &TextEdit::fileSaveAs);
    a->setPriority(QAction::LowPriority);
    menu->addSeparator();
*/
    const QIcon uriIcon =  QIcon::fromTheme("document-share", QIcon(rsrcPath + "/uri.png"));
    a = menu->addAction(uriIcon, tr("&Share URI..."), this, &TextEdit::shareURI);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    tb->addAction(a);
#ifndef QT_NO_PRINTER

    /*

    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    a = menu->addAction(printIcon, tr("&Print..."), this, &TextEdit::filePrint);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Print);
    tb->addAction(a);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    menu->addAction(filePrintIcon, tr("Print Preview..."), this, &TextEdit::filePrintPreview);
    */

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_P);
    tb->addAction(a);

    menu->addSeparator();
#endif

    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

void TextEdit::setupEditActions()
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));
    /*


    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();
*/
#ifndef QT_NO_CLIPBOARD
//    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
//    actionCut = menu->addAction(cutIcon, tr("&Cut"), textEdit, &QTextEdit::cut);
//    actionCut->setPriority(QAction::LowPriority);
//    actionCut->setShortcut(QKeySequence::Cut);
//    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), this, &TextEdit::processPaste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData()){
        actionPaste->setEnabled(md->hasText());
    }
#endif
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &TextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    /*
    //REMOVING COMBO BOX FOR LISTS
    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &TextEdit::textStyle);
    */

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

bool TextEdit::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        textEdit->setHtml(str);
    } else {
        str = QString::fromLocal8Bit(data);
        textEdit->setPlainText(str);
    }

    setCurrentFileName(f);
    return true;
}

bool TextEdit::maybeSave()
{
    //modified
    return true;

    if (!textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret =
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             tr("The document has been modified.\n"
                                "Do you want to save your changes?"),
                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::fileNew()
{
    if (maybeSave()) {
        textEdit->clear();
        //setCurrentFileName(QString());
    }
}

void TextEdit::logout(){
    tcpSocket->doDisconnect();
    hideWindow();
    showLoginWindow();
}

void TextEdit::setEditorList(QString list){
    textEdit->setEditorList(list);
}

void TextEdit::showTextEdit(ClientSocket* s){
    tcpSocket = s;
    connect(textEdit, &SharedEditor::packetReady,
            tcpSocket, &ClientSocket::writeMessage);
    connect(tcpSocket, &ClientSocket::signalMessage,
            textEdit, &SharedEditor::incomingPacket);
    this->fileNew();
    this->show();
}

void TextEdit::userInfoReceived(QString command){
    this->name=command.split(',')[0];
    this->siteID=command.split(',')[1];
    this->nick=command.split(',')[2];
    textEdit->setSiteId(this->siteID.toInt());
}

void TextEdit::changeFileName(QString filename){
    setCurrentFileName(filename);
}

void TextEdit::hideWindow(){
    disconnect(textEdit, &SharedEditor::packetReady,
            tcpSocket, &ClientSocket::writeMessage);
    disconnect(tcpSocket, &ClientSocket::signalMessage,
            textEdit, &SharedEditor::incomingPacket);
    this->hide();
}

void TextEdit::fileOpen()
{
    newFileListWindow(tcpSocket);
    /*
    QFileDialog fileDialog(this, tr("Open File..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList() << "text/html" << "text/plain");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (load(fn))
        statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fn)));
    else
        statusBar()->showMessage(tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(fn)));
    */
}

bool TextEdit::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success) {
        textEdit->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}

bool TextEdit::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("odt");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}


void TextEdit::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
#endif
}

void TextEdit::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif
}

void TextEdit::showConnectedUsers(){
    QMessageBox* msgAbout=new QMessageBox(this);
    msgAbout->setStyleSheet("QLabel{{min-width: 500px;}");
    msgAbout->setWindowTitle("Users");
    msgAbout->setText("Connected users to "+ fileName+"      ");
    QString infotext = "<span style='text-align: center'>";

    std::map<QString,int> usernameToSiteId=textEdit->getEditorList();

    for(auto pair : usernameToSiteId){
        qint32 siteId=pair.second;
        if(textEdit->siteIdHasColor(siteId))
            infotext+=("<p style='color: "+textEdit->getSiteIdColor(siteId).name(QColor::HexRgb)+";'>");
        else
            infotext+=("<p>");
        infotext.append(pair.first+"    ");
        infotext+=("</p>");
    }
    infotext+=("</span>");
    msgAbout->setDefaultButton(QMessageBox::Ok);
    msgAbout->setInformativeText(infotext);
    msgAbout->show();
}

void TextEdit::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}

void TextEdit::shareURI()
{
   QDialog* URIwin = new QDialog(this);
   Ui::ShareURI ui;
   ui.setupUi(URIwin);
   ui.uri->setReadOnly(true);
   QPalette *palette = new QPalette();
   palette->setColor(QPalette::Base,Qt::lightGray);
   palette->setColor(QPalette::Text,Qt::black);
   ui.uri->setPalette(*palette);
   QString uri = generateURI();
   ui.uri->setText(uri);
   URIwin->show();
}

QString TextEdit::generateURI() {
    QString str = QString::fromStdString(tcpSocket->getaddress());
    str.append("/"+fileName);
    return str;
}


void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    //HIDE CURSORS
    textEdit->hideUserCursors();

    textEdit->document()->print(&printer);

    //SHOW CURSORS
    textEdit->showUserCursors();

    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    QTextCursor cursor = textEdit->textCursor();
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    if(!cursor.hasSelection()){
        cursor.select(QTextCursor::WordUnderCursor);
    }
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    qDebug() << "cursorSelectionStart: " << start;
    qDebug() << "cursorSelectionEnd: " << end;
    if(cursor.position() > end)
        end = cursor.position();
    qDebug() << "cursor.position " << cursor.position();
    qDebug() << "cursorSelectionStart: " << start;
    qDebug() << "cursorSelectionEnd: " << end;
    if(actionTextBold->isChecked()){
        textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Bold));
    }
    else{
        textEdit->localUnsetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Bold));
    }
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    QTextCursor cursor = textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if(actionTextUnderline->isChecked()){
        textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Underlined));
    }
    else{
        textEdit->localUnsetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Underlined));
    }
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    QTextCursor cursor = textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if(actionTextItalic->isChecked()){
        textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Italic));
    }
    else{
        textEdit->localUnsetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::Italic));
    }
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    QString familyName = QFont(f).family();
    QTextCursor cursor = textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<int>(),StyleType::Font, familyName));
    QTextCharFormat fmt;
    fmt.setFontFamily(familyName);
    mergeFormatOnWordOrSelection(fmt);

}

void TextEdit::textSize(const QString &p)
{
    QTextCursor cursor = textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<qint32>(),StyleType::FontSize, p.toInt()));
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

/*
void TextEdit::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}
*/

void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCursor cursor = textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    textEdit->localSetStyle(start, end, Symbol(-1, -1, std::vector<int>(),StyleType::Color, col.name()));
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign(QAction *a)
{
    int position = textEdit->textCursor().position();
    if (a == actionAlignLeft){
        textEdit->localSetAlignment(position, AlignmentType::AlignLeft);
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    }
    else if (a == actionAlignCenter){
        textEdit->localSetAlignment(position, AlignmentType::AlignCenter);
        textEdit->setAlignment(Qt::AlignHCenter);
    }
    else if (a == actionAlignRight){
        textEdit->localSetAlignment(position, AlignmentType::AlignRight);
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    }
    else if (a == actionAlignJustify){
        textEdit->localSetAlignment(position, AlignmentType::AlignJustified);
        textEdit->setAlignment(Qt::AlignJustify);
    }
}

void TextEdit::accountInfo(){
    QDialog* accountInfo = new QDialog(this);
    accountInfo->setWindowTitle("Account Info");
    Ui::accountInfo ui;
    ui.setupUi(accountInfo);
    ui.name->setText(name);
    ui.name->setFont(QFont("Arial",10));
    ui.nick->setText(nick);
    ui.nick->setFont(QFont("Arial",10));
    ui.siteID->setText(siteID);
    ui.siteID->setFont(QFont("Arial",10));
    accountInfo->show();
}

void TextEdit::editNick(){
    bool ok;
    QString newNick = QInputDialog::getText(this,"EditNick","Your current nick is: "+nick+"\nInsert the new one:",QLineEdit::Normal,"", &ok);
    if(!ok) {
        return;
    }
    if(newNick.isEmpty()){
        QMessageBox::warning(this, "Error", "Please insert a nickname");
        return;
    }
    nick = newNick;
    Message m = Message{MessageType::EditNick,name+','+nick} ;
    textEdit->packetReady(m);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());



    /*
    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
        case QTextListFormat::ListDisc:
            comboStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            comboStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            comboStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            comboStyle->setCurrentIndex(4);
            break;
        case QTextListFormat::ListLowerAlpha:
            comboStyle->setCurrentIndex(5);
            break;
        case QTextListFormat::ListUpperAlpha:
            comboStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerRoman:
            comboStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperRoman:
            comboStyle->setCurrentIndex(8);
            break;
        default:
            comboStyle->setCurrentIndex(-1);
            break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);
    }
    */
}

void TextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void TextEdit::processPaste()
{
    if (const QMimeData *md = QApplication::clipboard()->mimeData()){
        textEdit->processPaste(md);
    }
}

void TextEdit::about()
{
    QMessageBox::about(this, tr("About"), tr("Kangaroo Docs\nA collaborative "
        "rich text editor that will make you jump from happiness."));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}
