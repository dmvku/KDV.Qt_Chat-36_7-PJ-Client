#include "registrationform.h"
#include "ui_registrationform.h"
#include "commonfunc.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>

RegistrationForm::RegistrationForm(Client* client, QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::RegistrationForm)    
{
    ui->setupUi(this);
    clientPtr_ = client;
    QRegularExpression Regexp("[a-zA-Z0-9_]+");
    ui->loginLineEdit->setValidator(new QRegularExpressionValidator(Regexp, ui->loginLineEdit));
    ui->nameLineEdit->setValidator(new QRegularExpressionValidator(Regexp, ui->nameLineEdit));
}

RegistrationForm::~RegistrationForm()
{    
    delete ui;
}

void RegistrationForm::setRegistrationRequest(QString status)
{
    {
        if(status.section("|", 0, 0).toInt() != static_cast<int>(CommonFunc::Commands::registerError_))
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
}

void RegistrationForm::on_loginPushButton_clicked()
{
    reject();
    emit loginFormRequest();
}

void RegistrationForm::on_confirmButtonBox_accepted()
{
    if(ui->nameLineEdit->text().isEmpty()
        || ui->loginLineEdit->text().isEmpty()
        || ui->passwordLineEdit->text().isEmpty()
        || ui->passwordConfirmLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this
                              , "Error"
                              , "Fields should not be empty");
        return;
    }

    if(ui->passwordLineEdit->text() != ui->passwordConfirmLineEdit->text())
    {
        QMessageBox::critical(this
                              , "Registration error"
                              , "Passwords is not match");
        return;
    }

    clientPtr_->registerUser(ui->loginLineEdit->text()
                             , ui->nameLineEdit->text()
                             , ui->passwordLineEdit->text());
}

void RegistrationForm::on_confirmButtonBox_rejected()
{
    reject();
}
