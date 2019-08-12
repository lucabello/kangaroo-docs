#ifndef FILELISTWINDOW_H
#define FILELISTWINDOW_H
#include <QMainWindow>
#include <QGroupBox>
#include <QLineEdit>
#include "ClientSocket.h"

class FileListWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FileListWindow(QWidget *parent = nullptr,ClientSocket *tcpSocket=nullptr,std::string fileList=nullptr);

signals:

public slots:
    void incomingPacket(Message message);

private slots:

private:
    ClientSocket *tcpSocket;
    std::string fileList;
    void openEditorWindow();
};

#endif // FILELISTWINDOW_H
