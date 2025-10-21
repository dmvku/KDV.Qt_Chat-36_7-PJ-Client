#ifndef COMMONFUNC_H
#define COMMONFUNC_H

#include <QString>

namespace CommonFunc
{
enum Commands
{
    setUserStatus_ = 10,
    clientLogout_ = 11,
    addUser_ = 20,
    registerUser_ = 21,
    registerDone_ = 22,
    registerError_ = 23,
    loginUser_ = 24,
    loginDone_ = 25,
    loginError_ = 26,
    logoutUser_ = 27,
    addMessage_ = 40,
    sendMessage_ = 41,
    sendMessageResult_ = 42,
    sendMessageDone_ = 43,
    sendMessageError_ = 44
};

enum User_Status
{
    isDeleted_ = 0,
    isActive_ = 1,
    isBanned_ = 2,
    //isAdmin_ = 3
};

bool checkPort(quint16 port);
bool checkAddressIP(const QString& addressIP);
}

#endif // COMMONFUNC_H
