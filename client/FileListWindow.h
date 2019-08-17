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
    explicit FileListWindow(QWidget *parent = nullptr);
signals:
    void showTextEdit(ClientSocket*);
    void changeFileName(QString);
    void setEditorList(QString);

private slots:
    void openFileClicked();
    void newFileClicked();
    void openURIClicked();

public slots:
    void incomingMessage(Message message);
    void showFileList(ClientSocket*, std::vector<std::string>);
    void newFileListWindow(ClientSocket*);
    void hideWindow();

private:
    ClientSocket *tcpSocket;
    QListWidget *qFileList;
    std::vector<std::string> fileList;
};

#endif // FILELISTWINDOW_H
