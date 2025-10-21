#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
	User() = default;
    User(int userID
         , const QString& name
         , short status);
	~User();
		
    int getUserID() const;
    const QString& getName() const; 
    short getStatus() const;

    void setUserID(int userID);
    void setName(QString name);
    void setStatus(short status);

private:
    int userID_;
    QString name_; 
    short status_;
};

#endif // USER_H
