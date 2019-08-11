#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QGroupBox>
#include <QLineEdit>

class Login : public QMainWindow
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = nullptr);

signals:

public slots:

private slots:
    void on_button_clicked();

private:
    QGroupBox *gb;
    QLineEdit *usernameLine;
    QLineEdit *passwordLine;
    void openEditorWindow();
};

#endif // LOGIN_H
