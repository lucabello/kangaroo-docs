#ifndef FILELISTWINDOW_H
#define FILELISTWINDOW_H
#include <QMainWindow>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include "ClientSocket.h"

class FileListWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FileListWindow(QWidget *parent = nullptr,ClientSocket *tcpSocket=nullptr,
                            std::vector<std::string> fileList=std::vector<std::string>());

private slots:
    void openFileClicked();
    void newFileClicked();

public slots:
    void incomingPacket(Message message);

private slots:

private:
    ClientSocket *tcpSocket;
    QListWidget *qFileList;
    std::vector<std::string> fileList;
    void openEditorWindow(QString fileName);
};

#endif // FILELISTWINDOW_H
