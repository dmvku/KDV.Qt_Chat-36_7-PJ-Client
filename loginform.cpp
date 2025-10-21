#include "loginform.h"
#include "ui_loginform.h"
#include "commonfunc.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>

LoginForm::LoginForm(Client* client, QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    clientPtr_ = client;
    QRegularExpression Regexp("[a-zA-Z0-9_]+");
    ui->loginLineEdit->setValidator(new QRegularExpressionValidator(Regexp, ui->loginLineEdit));
}

LoginForm::~LoginForm()
{    
    delete ui;
}

void LoginForm::getLoginRequest(QString status)
{
    if(status.section("|", 0, 0).toInt() != CommonFunc::Commands::loginError_)
    {
        QMessageBox::information(this
                                 , "Done"
                                 , status.section("|", 1, 1));

        accept();
    }
    else
    {
        QMessageBox::critical(this
                              , "Failed"
                              , status.section("|", 1, 1));
        return;
    }
}

void LoginForm::on_registrationPushButton_clicked()
{
    reject();
    emit registrationFormRequest();
}

void LoginForm::on_confirmButtonBox_accepted()
{
    if(ui->loginLineEdit->text().isEmpty()
        || ui->passwordLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this
                              , "Error"
                              , "Fields should not be empty");
        return;
    }

    clientPtr_->loginUser(ui->loginLineEdit->text()
                        , ui->passwordLineEdit->text());
}

void LoginForm::on_confirmButtonBox_rejected()
{
    reject();
}
