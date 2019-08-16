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
    ipLine->setText("127.0.0.1:1501");
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
    QString ip=ipLine->text().split(":").at(0);
    QString port=ipLine->text().split(":").at(1);

    //sanitize username and password to prevent errors
    //only alphanumeric characters are allowed
    username.remove(QRegExp("[^0-9a-zA-Z]"));
    password.remove(QRegExp("[^0-9a-zA-Z]"));

    tcpSocket = new ClientSocket(ip.toStdString(), port.toInt());
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &LoginWindow::incomingMessage);
    tcpSocket->doConnect();

    Message m {MessageType::Login,username+","+password}; //prepare and send message
    //char *serM = Message::serialize(m);
    tcpSocket->writeMessage(m);//serM,Symbol::peekIntFromByteArray(serM+4)+8);
}

void LoginWindow::registerClicked(){
    QString username=usernameLine->text();
    QString password=passwordLine->text();
    QString ip=ipLine->text().split(":").at(0);
    QString port=ipLine->text().split(":").at(1);

    tcpSocket = new ClientSocket(ip.toStdString(), port.toInt());
    connect(tcpSocket, &ClientSocket::signalMessage,
            this, &LoginWindow::incomingMessage);
    tcpSocket->doConnect();

    Message m {MessageType::Register,username+","+password};
    tcpSocket->writeMessage(m);
}

void LoginWindow::hideWindow(){
    disconnect(tcpSocket, &ClientSocket::signalMessage,
            this, &LoginWindow::incomingMessage);
    this->hide();
}

void LoginWindow::incomingMessage(Message message){
    qDebug()<<message.getCommand();
    switch(message.getType()){
        case MessageType::Login:
            siteIdReceived(message.getCommand().toUInt());
            showResult("Login successful.");
            break;
        case MessageType::Register:
            showResult("Registration successful.");
            break;
        case MessageType::FileList:
            openFileListWindow(message);
            break;
        case MessageType::Error:
            showResult(message);
            break;
        default:
            break;
    }
}

void LoginWindow::showResult(Message message){
    QMessageBox::information(this,"Result",message.getCommand());
}

void LoginWindow::showResult(QString result){
    QMessageBox::information(this,"Result",result);
}

void LoginWindow::openFileListWindow(Message message){
    QStringList qlist = message.getCommand().split(",");
    std::vector<std::string> fileList;
    for(QString s : qlist)
        fileList.push_back(s.toStdString());
    showFileList(tcpSocket, fileList);
}

