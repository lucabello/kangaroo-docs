#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>

#include "TextEdit.h"
#include "FileListWindow.h"

FileListWindow::FileListWindow(QWidget *parent,ClientSocket *tcpSocket,std::vector<std::string> fileList) : QMainWindow(parent),
    tcpSocket(tcpSocket),fileList(fileList)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    setWindowTitle(QCoreApplication::applicationName());

    resize(parent->size());
    move(parent->pos());

    connect(tcpSocket, &ClientSocket::incomingMessage,
            this, &FileListWindow::incomingPacket);

    QPushButton *buttonNewFile=new QPushButton(this);
    buttonNewFile->setText("New File");
    buttonNewFile->resize(this->width(),buttonNewFile->height());
    buttonNewFile->move(0,this->height()-buttonNewFile->height());

    QPushButton *buttonOpenFile=new QPushButton(this);
    buttonOpenFile->setText("Open File");
    buttonOpenFile->resize(this->width(),buttonOpenFile->height());
    buttonOpenFile->move(0,this->height()-buttonOpenFile->height()-buttonNewFile->height());

    qFileList=new QListWidget(this);
    qFileList->resize(this->width(),this->height()-buttonOpenFile->height()-buttonNewFile->height());

    for (std::string& fileName: fileList){
         qFileList->addItem(QString::fromStdString(fileName));
    }

    connect(buttonOpenFile, SIGNAL (released()), this, SLOT (openFileClicked()));
    connect(buttonNewFile, SIGNAL (released()), this, SLOT (newFileClicked()));

}

void FileListWindow::openFileClicked(){
    if(qFileList->currentItem()==nullptr)
        QMessageBox::warning(this,"title","No file selected");
    else{
        openEditorWindow(qFileList->currentItem()->text());
    }
}

void FileListWindow::newFileClicked(){
    openEditorWindow("new");
}

void FileListWindow::incomingPacket(Message message){
    qDebug() << "A new packet arrived! Message is the following.";
    qDebug() << QString::fromStdString(message.toString());
    switch(message.getType()){
        default:
            break;
    }
}

void FileListWindow::openEditorWindow(QString fileName){
    this->hide();

    //TUTTO PROVVISORIO -- problema socket! -- problema apertura nuove schede da far bene

    TextEdit *textEdit=new TextEdit(this);

    textEdit->resize(this->width(),this->height());
    textEdit->move(this->x(),this->y());
    if(fileName=="new")
        textEdit->fileNew();
    else {
        qDebug() << "Should open " << fileName;
        textEdit->fileNew();//OPEN FILE
    }

    textEdit->show();
}
