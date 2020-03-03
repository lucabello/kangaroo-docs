#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QInputDialog>

#include "TextEdit.h"
#include "FileListWindow.h"
#include <QDebug>
#include "ui_shareuri.h"

FileListWindow::FileListWindow(QWidget *parent) : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    setWindowTitle(QCoreApplication::applicationName());
//    resize(parent->size());
//    move(parent->pos());
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    resize(availableGeometry.width()/2, availableGeometry.height()/2);
//    move(QApplication::desktop()->screen()->rect().center());
//    this->setMaximumWidth(availableGeometry.width());
//    this->setMaximumHeight(availableGeometry.height());

    qFileList=new QListWidget(this);

    QPushButton *buttonNewFile=new QPushButton(this);
    buttonNewFile->setText("New File");
    buttonNewFile->resize(this->width(),buttonNewFile->height());
    buttonNewFile->move(0,this->height()-buttonNewFile->height());

    QPushButton *buttonOpenFile=new QPushButton(this);
    buttonOpenFile->setText("Open File");
    buttonOpenFile->resize(this->width(),buttonOpenFile->height());
    buttonOpenFile->move(0,this->height()-buttonOpenFile->height()-buttonNewFile->height());

    qFileList->setMinimumWidth(qFileList->sizeHintForColumn(0));
    qFileList->resize(this->width(),this->height()-buttonOpenFile->height()-buttonNewFile->height());


    connect(buttonOpenFile, SIGNAL (released()), this, SLOT (openFileClicked()));
    connect(buttonNewFile, SIGNAL (released()), this, SLOT (newFileClicked()));
}

void FileListWindow::openFileClicked(){
    if(qFileList->currentItem()==nullptr)
        QMessageBox::warning(this,"Warning","No file selected!");
    else{
        QString filename = qFileList->currentItem()->text();
        changeFileName(filename);
        Message m {MessageType::Open, filename};
        tcpSocket->writeMessage(m);
    }
}

void FileListWindow::newFileClicked(){
    QString filename = QInputDialog::getText(this,"New File Name","Insert the name of new file:");
    if(filename.isEmpty()){
        QMessageBox::warning(this, "Error", "Please insert something as filename.");
        return;
    }
    changeFileName(filename);
    Message m {MessageType::Create, filename};
    tcpSocket->writeMessage(m);
}

void FileListWindow::incomingMessage(Message message){
    qDebug() << "[FileListWindow] New Message arrived.";
    qDebug() << QString::fromStdString(message.toString());
    switch(message.getType()){
        case MessageType::Create:
        case MessageType::Open:
            showTextEdit(tcpSocket);
            break;
        case MessageType::FileList:
        {
            QStringList qlist = message.getCommand().split(",");
            std::vector<std::string> fileList;
            for(QString s : qlist)
                fileList.push_back(s.toStdString());
            showFileList(tcpSocket, fileList);
            break;
        }
        case MessageType::EditorList:
        {
            emit setEditorList(message.getCommand());
            break;
        }
        case MessageType::Error:
            qDebug() << "Error!" << message.getCommand();
            break;
        default:
            break;
    }
}

void FileListWindow::showFileList(ClientSocket* s, std::vector<std::string> files){
    tcpSocket = s;
    fileList = files;
    qFileList->clear();
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &FileListWindow::incomingMessage, Qt::UniqueConnection);
    for (std::string& fileName: fileList){
         qFileList->addItem(QString::fromStdString(fileName));
    }
    this->show();
}

void FileListWindow::newFileListWindow(ClientSocket *s){
    tcpSocket = s;
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &FileListWindow::incomingMessage, Qt::UniqueConnection);
    tcpSocket->writeMessage(Message{MessageType::FileList,""});
}

void FileListWindow::hideWindow(){
    disconnect(tcpSocket, &ClientSocket::signalMessage,
            this, &FileListWindow::incomingMessage);
    this->hide();
}
