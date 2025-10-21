#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>

class Message
{
public:
    Message();
    Message(int from
            , int to
            , QString message
            , QString createTime);

	~Message();

    int getFrom() const;
    int getTo() const;
    const QString getMessage() const;
    const QString getCreateTime() const;

    void setFrom(int from);
    void setTo(int to);
    void setMessage(QString message);
    void setCreateTime(QString createTime);

private:
    int from_;
    int to_;
    QString message_;
    QString createTime_;
};

#endif // MESSAGE_H
