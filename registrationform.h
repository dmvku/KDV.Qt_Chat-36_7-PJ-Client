#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include "client.h"

#include <QDialog>
#include <QWidget>

namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationForm(Client* client, QDialog *parent = nullptr);
    ~RegistrationForm();    
    void setRegistrationRequest(QString status);

signals:
    void loginFormRequest();

private slots:
    void on_loginPushButton_clicked();
    void on_confirmButtonBox_accepted();
    void on_confirmButtonBox_rejected();

private:
    Ui::RegistrationForm *ui;
    Client* clientPtr_;
};

#endif // REGISTRATIONFORM_H
