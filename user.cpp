#include "user.h"

User::User(int userID
           , const QString& name
           , short status)
           : userID_{ userID }
           , name_{ name }
           , status_{ status }
{

}

User::~User()
{

}

int User::getUserID() const
{
    return userID_;
}

const QString& User::getName() const
{
    return name_;
}

short User::getStatus() const
{
    return status_;
}

void User::setUserID(int userID)
{
    userID_ = userID;
}

void User::setName(QString name)
{
    name_ = name;
}

void User::setStatus(short status)
{
    status_ = status;
}
