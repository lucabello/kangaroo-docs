#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QGroupBox>
#include <QLineEdit>
#include "ClientSocket.h"

class LoginWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:

public slots:
    void incomingPacket(Message message);

private slots:
    void on_button_clicked();

private:
    QGroupBox *gb;
    QLineEdit *usernameLine;
    QLineEdit *passwordLine;
    QLineEdit *ipLine;
    ClientSocket *tcpSocket;
    void openEditorWindow();
    void login(Message message);
};

#endif // LOGIN_H
