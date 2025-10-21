#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commonfunc.h"

#include <QMessageBox>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    ui->actionLogin->setEnabled(false);
    ui->actionRegistration->setEnabled(false);
    ui->actionLogout->setEnabled(false);
    ui->sendMessagelineEdit->setEnabled(false);    
    ui->sendButton->setEnabled(false);
    ui->privateOnlyCheckBox->setEnabled(false);
    ui->userOnlyCheckBox->setEnabled(false);

    QRegularExpression messageRegexp("[^|#]*");
    ui->sendMessagelineEdit->setValidator(new QRegularExpressionValidator(messageRegexp, ui->sendMessagelineEdit));

    client_ = new Client;

    currentUserPtr_ = client_->getCurrentUser();

    ui->IPAddressValue->setText(client_->getServerAddress());
    ui->portValue->setText(QString::number(client_->getServerPort()));

    connect(client_, &Client::displayConnectionStatus, this, &MainWindow::disconnectClient);    
    connect(client_, &Client::addItemToChatWindow, this, &MainWindow::addMessageToChatWindow);
    connect(client_, &Client::addMessage, this, &MainWindow::addMessageToChatWindow);
    connect(client_, &Client::addIntoUsersList, this, &MainWindow::addUserToWindow);
    connect(client_, &Client::acceptCurrentUserStatus, this, &MainWindow::applyCurrentUserStatus);
    connect(client_, &Client::addServiceMessage, this, &MainWindow::addServiceMessageToChatWindow);
    connect(client_, &Client::sendMessageResult, this, &MainWindow::sendMessageResultInfo);
    connect(client_, &Client::applyLoginResult, this, &MainWindow::applyLoginStatus);    
    connect(client_, &Client::applyRegisterResult, this, &MainWindow::applyLoginStatus);    
}

MainWindow::~MainWindow()
{
    if(loginWiget_) loginWiget_->deleteLater();
    if(registrationWiget_) registrationWiget_->deleteLater();
    delete client_;
    delete ui;
}

void MainWindow::disconnectClient()
{
    applyLoginStatus(QString::number(CommonFunc::Commands::logoutUser_));
    setOnlineStatus(false);
}

void MainWindow::on_sendButton_clicked()
{
    if(ui->sendMessagelineEdit->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "Message should not be empty");
    }
    if(isPrivateMessage)
    {
        client_->sendMessage(ui->sendMessagelineEdit->text(), ui->userListWidget->currentItem()->text());        
    }
    else
    {
        client_->sendMessage(ui->sendMessagelineEdit->text(), "0");
    }
}

void MainWindow::sendMessageResultInfo(QString result)
{
    if(result.section(delimiter_, 0, 0).toInt() == CommonFunc::sendMessageDone_)
    {
        QMessageBox::information(this, "Done", result.section(delimiter_, 1, 1));

        ui->sendMessagelineEdit->clear();
    }
    else
    {
        QMessageBox::critical(this, "Fault", result.section(delimiter_, 1, 1));
    }
}

void MainWindow::on_connectButton_clicked()
{
    if(!client_->getConnectionStatus())
    {
        quint16 port = ui->portValue->text().toUShort();
        if(!CommonFunc::checkAddressIP(ui->IPAddressValue->text()))
        {
            QMessageBox::critical(this, "Error", "Invalid IP address");
            ui->IPAddressValue->clear();
            return;
        }

        if(!CommonFunc::checkPort(port))
        {
            QMessageBox::critical(this, "Error", "Invalid port");
            ui->portValue->clear();
            return;
        }

        bool isConnected = client_->startConnection(ui->IPAddressValue->text(), port);
        if(!isConnected)
        {
            QMessageBox::critical(this, "Connection failure"
                                     , "The server\n"
                                     + ui->IPAddressValue->text().toUtf8()
                                     + ":" + ui->portValue->text().toUtf8()
                                     + "\nis not responding");
            return;
        }
        else
        {
            setOnlineStatus(isConnected);
        }
    }
    else
    {
        client_->stopConnection();
        setOnlineStatus(false);
        QMessageBox::information(this, "Warning", "Connection close");
    }
}

void MainWindow::on_sendMessagelineEdit_returnPressed()
{
    on_sendButton_clicked();
}

void MainWindow::on_userListWidget_itemClicked(QListWidgetItem *item)
{
    if(isPrivateMessage && recipient_ == item->text())
    {
        isPrivateMessage = false;
        item->setSelected(false);
        recipient_ = nullptr;
        ui->sendButton->setText("Send");        
        ui->userOnlyCheckBox->setEnabled(false);
        client_->setSelectedUserID(0);
        ui->userOnlyCheckBox->setChecked(false);
    }
    else
    {
        isPrivateMessage = true;
        recipient_ = item->text();
        ui->sendButton->setText("Send to " + recipient_);
        ui->userOnlyCheckBox->setEnabled(true);
        ui->chatUsersLable->setText(recipient_);
        client_->setSelectedUserID(item->text());
    }

    if(isPrivate_ || onlyForSelectedUser_)
    {
        ui->chatWindow->clear();
        client_->showChatWindow();
    }
}

void MainWindow::on_actionLogin_triggered()
{
    loginWiget_ = new LoginForm(client_);
    loginWiget_->setAttribute(Qt::WA_DeleteOnClose);
    connect(loginWiget_, &LoginForm::registrationFormRequest,
            this, &MainWindow::on_actionRegistration_triggered);
    connect(client_, &Client::loginError,
            loginWiget_, &LoginForm::getLoginRequest, Qt::UniqueConnection);
    connect(client_, &Client::informingAboutLogin,
            loginWiget_, &LoginForm::getLoginRequest, Qt::UniqueConnection);
    loginWiget_->setWindowModality(Qt::ApplicationModal);
    loginWiget_->show();
}

void MainWindow::on_actionRegistration_triggered()
{
    registrationWiget_ = new RegistrationForm(client_);
    registrationWiget_->setAttribute(Qt::WA_DeleteOnClose);
    connect(registrationWiget_, &RegistrationForm::loginFormRequest,
            this, &::MainWindow::on_actionLogin_triggered);
    connect(client_, &Client::registerError,
            registrationWiget_, &RegistrationForm::setRegistrationRequest);
    connect(client_, &Client::informingAboutRegister,
            registrationWiget_, &RegistrationForm::setRegistrationRequest);
    registrationWiget_->setWindowModality(Qt::ApplicationModal);
    registrationWiget_->show();
}

void MainWindow::on_actionLogout_triggered()
{

    applyLoginStatus(QString::number(CommonFunc::Commands::logoutUser_));
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::setOnlineStatus(bool connectionStatus)
{

    ui->actionLogin->setEnabled(connectionStatus);
    ui->actionRegistration->setEnabled(connectionStatus);
    ui->actionLogout->setEnabled(connectionStatus);

    if(connectionStatus)
    {
        ui->informationLabel->setText("<font color=green>Connected</font>");
        ui->connectButton->setStyleSheet("color: red;");
        ui->connectButton->setText("Disconnect");
    }
    else
    {        
        ui->informationLabel->setText("<font color=red>Disconnected</font>");
        ui->connectButton->setStyleSheet("color: green;");
        ui->connectButton->setText("Connect");
        ui->userListWidget->clear();
        ui->chatWindow->clear();
    }
}

void MainWindow::addMessageToChatWindow(QString from, QString to, QString message, QString createTime)
{
    bool itsFromMe { currentUserPtr_->getName() == from };
    bool itsToMe { currentUserPtr_->getName() == to };

    QString sender = itsFromMe ? "I'm" : from;
    QString printTo = to == "" ? "write" : "to ";
    QString recipient = itsToMe ? "me: " : (to == "" ? "" : to);

    ui->chatWindow->append(QString("<font color=\"purple\">%1</font>  <font color=\"blue\">%2</font> %3 "
                              "<font color=\"blue\">%4</font>: "
                              "<span style=\"font-style:%5; color:%6;\">%7</span>")
                               .arg(createTime
                                    , sender
                                    , printTo
                                    , recipient
                                    , itsFromMe || itsToMe ? "italic" : "normal"
                                    , itsFromMe || itsToMe  ? "green" : "black"
                                    , message));
}

void MainWindow::addServiceMessageToChatWindow(QString serviceMessage)
{
    ui->chatWindow->append(serviceMessage);
}

void MainWindow::addUserToWindow(QString name)
{
    ui->userListWidget->addItem(name);
}

void MainWindow::applyCurrentUserStatus()
{
    if(currentUserPtr_->getStatus() == CommonFunc::User_Status::isBanned_)
    {
        ui->sendMessagelineEdit->setEnabled(false);
        ui->sendButton->setEnabled(false);
        ui->chatLable->setText("Chat: <font color=red>"
                               + currentUserPtr_->getName()
                               + " in ban</font>");        
    }
    else
    {
        ui->sendMessagelineEdit->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->chatLable->setText("Chat: <font color=green>"
                               + currentUserPtr_->getName()
                               + "</font>");
    }
}

void MainWindow::applyLoginStatus(QString status)
{
    bool logoutStatus = (status.toInt() == CommonFunc::Commands::logoutUser_);

    ui->sendMessagelineEdit->setEnabled(!logoutStatus);
    ui->sendButton->setEnabled(!logoutStatus);
    ui->actionLogin->setEnabled(logoutStatus);
    ui->actionRegistration->setEnabled(logoutStatus);
    ui->actionLogout->setEnabled(!logoutStatus);
    ui->privateOnlyCheckBox->setEnabled(!logoutStatus);


    if(logoutStatus)
    {
        ui->chatWindow->clear();
        ui->userListWidget->clear();
        ui->chatLable->setText("Chat");
        currentUserPtr_->setUserID(0);
        currentUserPtr_->setName("");
        currentUserPtr_->setStatus(0);
        this->setWindowTitle("Chat (client)");
        client_->clearData();
        client_->logoutClient();
    }
    else
    {
        applyCurrentUserStatus();
        this->setWindowTitle("Chat (" + currentUserPtr_->getName() + ")");
    }
}

void MainWindow::on_userOnlyCheckBox_toggled(bool checked)
{
    if(checked)
    {
        onlyForSelectedUser_ = true;
        client_->setIsSelectedUserOnly(true);
        ui->chatWindow->clear();
        client_->showChatWindow();
    }
    else
    {
        onlyForSelectedUser_ = false;
        client_->setIsSelectedUserOnly(false);
        ui->chatWindow->clear();
        client_->showChatWindow();
    }
}

void MainWindow::on_privateOnlyCheckBox_toggled(bool checked)
{
    if(checked)
    {
        isPrivate_ = true;
        client_->setIsPrivateChat(true);
        ui->chatWindow->clear();
        client_->showChatWindow();
    }
    else
    {
        isPrivate_ = false;
        client_->setIsPrivateChat(false);
        ui->chatWindow->clear();
        client_->showChatWindow();
    }
}
