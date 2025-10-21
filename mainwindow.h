#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "loginform.h"
#include "registrationform.h"
#include <QMainWindow>
#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

signals:    
    void changeRecipient();
    void logoutClient(QString status);    

public slots:
    void disconnectClient();

private slots:
    void on_sendButton_clicked();
    void on_connectButton_clicked();
    void on_sendMessagelineEdit_returnPressed();
    void on_userListWidget_itemClicked(QListWidgetItem *item);
    void on_actionLogin_triggered();
    void on_actionRegistration_triggered();
    void on_actionLogout_triggered();
    void on_actionExit_triggered();
    void setOnlineStatus(bool connectionStatus);
    void applyLoginStatus(QString status);
    void on_userOnlyCheckBox_toggled(bool checked);
    void on_privateOnlyCheckBox_toggled(bool checked);
    void addMessageToChatWindow(QString from, QString to
                                , QString message, QString createTime);
    void addServiceMessageToChatWindow(QString serviceMessage);
    void sendMessageResultInfo(QString result);
    void addUserToWindow(QString name);


private:
    Ui::MainWindow *ui;        
    LoginForm* loginWiget_ = nullptr;
    RegistrationForm* registrationWiget_ = nullptr;
    Client* client_;
    User* currentUserPtr_;
    bool isPrivate_{ false };
    bool onlyForSelectedUser_{ false };
    bool isPrivateMessage { false };
    QString recipient_;
    QString delimiter_ = "|";

    void applyCurrentUserStatus();
};

#endif // MAINWINDOW_H
