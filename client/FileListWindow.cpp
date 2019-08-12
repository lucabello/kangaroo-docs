#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "TextEdit.h"
#include "FileListWindow.h"

FileListWindow::FileListWindow(QWidget *parent,ClientSocket *tcpSocket,std::string fileList) : QMainWindow(parent),
    tcpSocket(tcpSocket),fileList(fileList)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());
    connect(tcpSocket, &ClientSocket::incomingMessage,
            this, &FileListWindow::incomingPacket);

    qDebug()<<QString::fromStdString(fileList);
}

void FileListWindow::incomingPacket(Message message){
    qDebug() << "A new packet arrived! Message is the following.";
    qDebug() << QString::fromStdString(message.toString());
    switch(message.getType()){
        default:
            break;
    }
}

void FileListWindow::openEditorWindow(){
    this->hide();

    TextEdit *textEdit=new TextEdit(this);

    textEdit->resize(this->width(),this->height());
    textEdit->move(this->x(),this->y());

    textEdit->fileNew();

    textEdit->show();
}
