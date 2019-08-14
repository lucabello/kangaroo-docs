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
    void showFileList(ClientSocket*, std::vector<std::string>);
    void siteIdReceived(int);

public slots:
    void incomingMessage(Message message);

private slots:
    void loginClicked();
    void registerClicked();
    void hideWindow();

private:
    QLineEdit *usernameLine;
    QLineEdit *passwordLine;
    QLineEdit *ipLine;
    ClientSocket *tcpSocket;
    void showResult(Message message);
    void showResult(std::string result);
    void openFileListWindow(Message message);
};

#endif // LOGIN_H