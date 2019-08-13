#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDesktopWidget>

#include "LoginWindow.h"
#include "FileListWindow.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    move((availableGeometry.width() - this->width()) / 2,
            (availableGeometry.height() - this->height()) / 2);

    QLabel *usernameLabel=new QLabel(this);
    usernameLabel->setText("Username:");

    usernameLine=new QLineEdit(this);
    usernameLine->move(usernameLabel->width(),0);

    QLabel *passwordLabel=new QLabel(this);
    passwordLabel->setText("Password:");
    passwordLabel->move(0,usernameLabel->height());

    passwordLine=new QLineEdit(this);
    passwordLine->move(passwordLabel->width(),passwordLabel->y());

    QLabel *ipLabel=new QLabel(this);
    ipLabel->setText("IP:");
    ipLabel->move(0,passwordLabel->y()+passwordLabel->height());

    ipLine=new QLineEdit(this);
    ipLine->setText("127.0.0.1");
    ipLine->move(ipLabel->width(),ipLabel->y());

    QPushButton *buttonLogin=new QPushButton(this);
    buttonLogin->setText("Login");
    buttonLogin->move(0,ipLabel->y()+ipLabel->height());
    QPushButton *buttonRegister=new QPushButton(this);
    buttonRegister->setText("Register");
    buttonRegister->move(buttonLogin->width(),buttonLogin->y());

    int width=usernameLabel->width()+usernameLine->width();
    int height=buttonLogin->y()+buttonLogin->height();
    int offsetX=(this->width()-width)/2;
    int offsetY=(this->height()-height)/2;

    usernameLabel->move(usernameLabel->x()+offsetX,usernameLabel->y()+offsetY);
    usernameLine->move(usernameLine->x()+offsetX,usernameLine->y()+offsetY);
    passwordLabel->move(passwordLabel->x()+offsetX,passwordLabel->y()+offsetY);
    passwordLine->move(passwordLine->x()+offsetX,passwordLine->y()+offsetY);
    ipLabel->move(ipLabel->x()+offsetX,ipLabel->y()+offsetY);
    ipLine->move(ipLine->x()+offsetX,ipLine->y()+offsetY);
    buttonLogin->move(buttonLogin->x()+offsetX,buttonLogin->y()+offsetY);
    buttonRegister->move(buttonRegister->x()+offsetX,buttonRegister->y()+offsetY);
    buttonLogin->resize(width/2,buttonLogin->height());
    buttonRegister->resize(width/2,buttonRegister->height());

    connect(buttonLogin, SIGNAL (released()), this, SLOT (loginClicked()));
    connect(buttonRegister, SIGNAL (released()), this, SLOT (registerClicked()));
}

void LoginWindow::loginClicked(){
    QString username=usernameLine->text();
    QString password=passwordLine->text();
    QString ip=ipLine->text();

    tcpSocket = new ClientSocket(ip.toStdString(), 1501);
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &LoginWindow::incomingMessage);

    tcpSocket->doConnect();

    Message m {MessageType::Login,"username:"+username.toStdString()+",password:"+password.toStdString()}; //prepare and send message
    //char *serM = Message::serialize(m);
    tcpSocket->writeMessage(m);//serM,Symbol::peekIntFromByteArray(serM+4)+8);
}

void LoginWindow::registerClicked(){
    QString username=usernameLine->text();
    QString password=passwordLine->text();
    QString ip=ipLine->text();

    tcpSocket = new ClientSocket(ip.toStdString(), 1501);
    tcpSocket->doConnect();
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &LoginWindow::incomingMessage);

    Message m {MessageType::Register,"username:"+username.toStdString()+",password:"+password.toStdString()};
    tcpSocket->writeMessage(m);
}

void LoginWindow::incomingMessage(Message message){
    qDebug()<<QString::fromStdString(message.getCommand());
    switch(message.getType()){
        case MessageType::Login:
        case MessageType::Register:
            showResult(message);
            break;
        case MessageType::FileList:
            openFileListWindow(message);
            break;
        default:
            break;
    }
}

void LoginWindow::showResult(Message message){
    QMessageBox::information(this,"Result",QString::fromStdString(message.getCommand()));
}

void LoginWindow::openFileListWindow(Message message){
    this->hide();
    std::vector<std::string> fileList=Message::split(message.getCommand(),",");

    FileListWindow *fileListWindow=new FileListWindow(this,tcpSocket,fileList);

    fileListWindow->show();
}

