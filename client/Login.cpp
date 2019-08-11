#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "Login.h"
#include "TextEdit.h"

Login::Login(QWidget *parent) : QMainWindow(parent)
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

    QPushButton *button=new QPushButton(gb);
    button->setText("Login");
    button->move((gb->width()-button->width())/2,passwordLine->y()+passwordLine->height());

    connect(button, SIGNAL (released()), this, SLOT (on_button_clicked()));

    gb->setFocus();
}

void Login::on_button_clicked(){
    QString username=usernameLine->text();
    QString password=passwordLine->text();

    if(username=="test"&&password=="test")
        openEditorWindow();
    else
        QMessageBox::warning(this,"Login","Username and password are not correct");

}

void Login::openEditorWindow(){
    this->hide();

    TextEdit *textEdit=new TextEdit(this);

    textEdit->resize(this->width(),this->height());
    textEdit->move(this->x(),this->y());

    textEdit->fileNew();

    textEdit->show();
}

