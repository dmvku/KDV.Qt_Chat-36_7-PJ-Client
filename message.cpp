#include "message.h"

Message::Message()
{

}

Message::Message(int from
                 , int to
                 , QString message
                 , QString createTime)
                 : from_{ from }
                 , to_{ to }
                 , message_{ message }
                 , createTime_{ createTime }
{

}

Message::~Message()
{

}

int Message::getFrom() const
{
    return from_;
}

int Message::getTo() const
{
    return to_;
}

const QString Message::getMessage() const
{
    return message_;
}

const QString Message::getCreateTime() const
{
    return createTime_;
}

void Message::setFrom(int from)
{
    from_ = from;
}

void Message::setTo(int to)
{
    to_ = to;
}

void Message::setMessage(QString message)
{
    message_ = message;
}

void Message::setCreateTime(QString createTime)
{
    createTime_ = createTime;
}
