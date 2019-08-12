#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "LoginWindow.h"
#include "FileListWindow.h"

LoginWindow::LoginWindow(QWidget *parent) : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    gb=new QGroupBox(this);
    gb->setTitle("SignIn");
    gb->resize(this->width(),this->height());
    setCentralWidget(gb);

    QLabel *usernameLabel=new QLabel(gb);
    usernameLabel->setText("Username:");
    usernameLabel->move((gb->width()-usernameLabel->width())/2,(gb->height()-usernameLabel->height())/2);

    usernameLine=new QLineEdit(gb);
    usernameLine->move(usernameLabel->x()+usernameLabel->width(),usernameLabel->y());

    QLabel *passwordLabel=new QLabel(gb);
    passwordLabel->setText("Password:");
    passwordLabel->move(usernameLabel->x(),usernameLabel->y()+usernameLabel->height());

    passwordLine=new QLineEdit(gb);
    passwordLine->move(passwordLabel->x()+passwordLabel->width(),passwordLabel->y());

    QLabel *ipLabel=new QLabel(gb);
    ipLabel->setText("IP:");
    ipLabel->move(passwordLabel->x(),passwordLabel->y()+passwordLabel->height());

    ipLine=new QLineEdit(gb);
    ipLine->move(ipLabel->x()+ipLabel->width(),ipLabel->y());
    ipLine->setText("127.0.0.1");

    QPushButton *button=new QPushButton(gb);
    button->setText("Login");
    button->move((gb->width()-button->width())/2,ipLine->y()+ipLine->height());

    connect(button, SIGNAL (released()), this, SLOT (on_button_clicked()));

    gb->setFocus();
}

void LoginWindow::on_button_clicked(){
    QString username=usernameLine->text();
    QString password=passwordLine->text();
    QString ip=ipLine->text();

    tcpSocket = new ClientSocket(ip.toStdString(), 1501);
    tcpSocket->doConnect();
    connect(tcpSocket, &ClientSocket::incomingMessage,
            this, &LoginWindow::incomingPacket);

    Message m {MessageType::Login,"username:"+username.toStdString()+",password:"+password.toStdString()}; //prepare and send message
    char *serM = Message::serialize(m);
    tcpSocket->writeData(serM,Symbol::peekIntFromByteArray(serM+4)+8);

    /*if(username=="test"&&password=="test")
        openEditorWindow();
    else
        QMessageBox::warning(this,"Login","Username and password are not correct");*/

}

void LoginWindow::incomingPacket(Message message){
    qDebug() << "A new packet arrived! Message is the following.";
    qDebug() << QString::fromStdString(message.toString());
    switch(message.getType()){
        case MessageType::Login:
            login(message);
            break;
        case MessageType::FileList:
            openFileListWindow(message);
            break;
        default:
            break;
    }
}

void LoginWindow::login(Message message){
    bool result=message.getCommand()=="true";
    if(result==true)
        QMessageBox::information(this,"Login","Login Successfull");
    else
        QMessageBox::warning(this,"Login","Username and password are not correct. Try again.");
}

void LoginWindow::openFileListWindow(Message message){
    this->hide();
    std::string fileList=message.getCommand();

    FileListWindow *fileListWindow=new FileListWindow(this,tcpSocket,fileList);

    fileListWindow->resize(this->width(),this->height());
    fileListWindow->move(this->x(),this->y());

    fileListWindow->show();
}

