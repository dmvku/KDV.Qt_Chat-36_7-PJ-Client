#pragma once

#include "socket.h"
#include "user.h"
#include "message.h"

#include <QObject>
#include <QString>
#include <QVector>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);
    ~Client();

    void loginUser(const QString login, const QString password);
    void registerUser(const QString name, const QString login, const QString password);
    void sendMessage(QString message, QString recipient);    
    bool startConnection(QString addressIP, quint16 port);
    void stopConnection();
    void setIsPrivateChat(bool isPrivate);
    void setIsSelectedUserOnly(bool isForSelectedUserOnly);
    void setSelectedUserID(QString selectedName);
    int getUserIDByName(const QString& userName);
    const QString getServerAddress() const;
     quint16 getServerPort() const;
     bool getConnectionStatus() const;
    User* getCurrentUser() const;
    QString getNameByUserID(int userID) const;
    QVector<User>* getChatUsers();
    QVector<Message>* getChatMessages();
    void clearData();
    void logoutClient();

signals:
    // in
    void addIntoUsersList(QString name);
    void displayConnectionStatus();    
    void addMessage(QString from, QString to, QString message, QString createTime);
    void addServiceMessage(QString serviceMessage);

    // out
    void registerError(QString result);
    void informingAboutRegister(QString result);
    void applyLoginResult(QString result);
    void applyRegisterResult(QString result);
    void loginError(QString result);
    void informingAboutLogin(QString result);
    void sendMessageResult(QString result);
    void addItemToChatWindow(QString from, QString to, QString message, QString createTime);
    void addItemToUserWindow(QString name);
    void acceptCurrentUserStatus();

public slots:
    void dataParsing(QString data);
    void showChatWindow();    

private:
    QString connectionConfigFile_{"connection.config"};
    quint16 serverPort_{ 7777 };
    QString serverAddress_{ "127.0.0.1" };
    Socket* socket_;
    User* currentUser_;
    QVector<User> chatUsers_;
    QVector<Message> chatMessages_;
    bool connectionStatus_{ false };    
    QString delimiter_ { "|" };    
    int selectedUserID_{};
    bool isPrivateChat_{ false };
    bool isSelectedUserOnly_{ false };

    void setCurrentUserData(QString userData);
    void addNewUser(QString userData);
    void changeUserStatus(QString userData);
    void addNewMessage(QString message);
    void setOnlineStatus(bool isConnect);    
    void getConnectionConfig();
    void saveConnectionConfig();    
};
