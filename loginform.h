#ifndef LOGINFORM_H
#define LOGINFORM_H

#include "client.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(Client* client, QDialog *parent = nullptr);
    ~LoginForm();
    void getLoginRequest(QString status);

signals:
    void registrationFormRequest();    

private slots:
    void on_registrationPushButton_clicked();
    void on_confirmButtonBox_accepted();
    void on_confirmButtonBox_rejected();

private:
    Ui::LoginForm *ui;    
    Client* clientPtr_;
};

#endif // LOGINFORM_H
