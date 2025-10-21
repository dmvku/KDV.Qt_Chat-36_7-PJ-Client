#include "client.h"
#include "sha1.h"

#include <QFile>
#include <QTime>
#include <QMessageBox>

Client::Client(QObject *parent)
    : QObject(parent)
{
    getConnectionConfig();

    socket_ = new Socket(serverAddress_, serverPort_);
    currentUser_ = new User;
    connect(socket_, &Socket::dataIsReady, this, &Client::dataParsing);
    connect(socket_, &Socket::setDisconnectStatus, this, &Client::setOnlineStatus);
}

Client::~Client()
{
    delete currentUser_;
    delete socket_;
    saveConnectionConfig();
}

void Client::loginUser(const QString login, const QString password)
{    
    QString passwordHash = QString::fromStdString(sha1(password.toStdString()));
    QString serverQuery = QString::number(static_cast<int>(CommonFunc::Commands::loginUser_))
                          + delimiter_ + login
                          + delimiter_ + passwordHash;

    socket_->dataTransmission(serverQuery);
}

void Client::registerUser(const QString login, const QString name, const QString password)
{
    QString passwordHash = QString::fromStdString(sha1(password.toStdString()));
    currentUser_->setName(name);
    QString serverQuery = QString::number(CommonFunc::Commands::registerUser_)
                          + delimiter_ + login
                          + delimiter_ + name
                          + delimiter_ + passwordHash;

    socket_->dataTransmission(serverQuery);
}

int Client::getUserIDByName(const QString& userName)
{
    for (auto& element : chatUsers_)
    {
        if (element.getName() == userName)
        {
            return element.getUserID();
        }
    }
    return {};
}

bool Client::startConnection(QString addressIP, quint16 port)
{
    connectionStatus_ = socket_->connectionToTheServer(addressIP, port);
    if(connectionStatus_)
    {
        saveConnectionConfig();
    }
    return connectionStatus_;
}

void Client::stopConnection()
{
    socket_->disconnecting();
}

void Client::setIsPrivateChat(bool isPrivate)
{
    isPrivateChat_ = isPrivate;
}

void Client::setIsSelectedUserOnly(bool isForSelectedUserOnly)
{
    isSelectedUserOnly_ = isForSelectedUserOnly;
}

void Client::setSelectedUserID(QString selectedName)
{
    selectedUserID_ = getUserIDByName(selectedName);
}

const QString Client::getServerAddress() const
{
    return serverAddress_;
}

quint16 Client::getServerPort() const
{
    return serverPort_;
}

QString Client::getNameByUserID(int userID) const
{
    for (auto& element : chatUsers_)
    {
        if (element.getUserID() == userID)
        {
            return element.getName();
        }
    }
    return "";
}

bool Client::getConnectionStatus() const
{
    return connectionStatus_;
}

QVector<User> *Client::getChatUsers()
{
    return &chatUsers_;
}

QVector<Message>* Client::getChatMessages()
{
    return &chatMessages_;
}

User* Client::getCurrentUser() const
{
    return currentUser_;
}

void Client::sendMessage(QString message, QString recipient)
{
    int from = currentUser_->getUserID();
    int to = getUserIDByName(recipient);

    socket_->dataTransmission(QString::number(static_cast<int>(CommonFunc::Commands::sendMessage_))
                              + delimiter_ + QString::number(from)
                              + delimiter_ + QString::number(to) + delimiter_ + message
                              + delimiter_ + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
}

void Client::dataParsing(QString data)
{    
    int command = data.section(delimiter_, 0, 0).toInt();
    switch (command)
    {
    case CommonFunc::Commands::setUserStatus_:
        changeUserStatus(data);
        break;
    case CommonFunc::Commands::addUser_:
        addNewUser(data);
        break;
    case CommonFunc::Commands::registerUser_:
    {
        emit addServiceMessage(data.section(delimiter_, 4, 4));
        if(!(data.section(delimiter_, 2, 2) == currentUser_->getName()))
        {
            addNewUser(data.section(delimiter_, 0, 3));
        }
        break;
    }
    case CommonFunc::Commands::registerDone_:
        setCurrentUserData(data);
        emit applyRegisterResult(data.section(delimiter_, 0, 0));
        emit informingAboutRegister (data.section(delimiter_, 0, 1));
        break;
    case CommonFunc::Commands::registerError_:
        emit registerError(data);
        break;
    case CommonFunc::Commands::loginUser_:
        emit addServiceMessage(data.section(delimiter_, 1, 1));
        break;
    case CommonFunc::Commands::loginDone_:
        setCurrentUserData(data);
        emit applyLoginResult(data.section(delimiter_, 0, 0));
        emit informingAboutLogin (data.section(delimiter_, 0, 1));
        break;
    case CommonFunc::Commands::loginError_:
        emit loginError(data);
        break;
    case CommonFunc::Commands::logoutUser_:
        emit addServiceMessage(data.section(delimiter_, 1, 1));
        break;
    case CommonFunc::Commands::addMessage_:    
        addNewMessage(data);
        break;  
    case CommonFunc::Commands::sendMessageResult_:
        emit sendMessageResult(data.section(delimiter_, 1, 2));
        break;    
    default:
        break;
    }
}

void Client::setCurrentUserData(QString userData)
{
    currentUser_->setUserID(userData.section(delimiter_, 2, 2).toInt());
    currentUser_->setName(userData.section(delimiter_, 3, 3));
    currentUser_->setStatus(userData.section(delimiter_, 4, 4).toShort());
    //emit acceptCurrentUserStatus();
}

void Client::addNewUser(QString userData)
{
    User tempUser;
    tempUser.setUserID(userData.section(delimiter_, 1, 1).toInt());
    tempUser.setName(userData.section(delimiter_, 2, 2));
    tempUser.setStatus(userData.section(delimiter_, 3, 3).toShort());
    emit addIntoUsersList(tempUser.getName());
    chatUsers_.push_back(tempUser);
}

void Client::changeUserStatus(QString userData)
{
    int userID = userData.section(delimiter_, 1, 1).toInt();
    short status = userData.section(delimiter_, 2, 2).toShort();
    for(auto& user : chatUsers_)
    {
        if (user.getUserID() == userID)
        {
            user.setStatus(status);
            emit addServiceMessage(userData.section(delimiter_, 3, 3));
            if(userID == currentUser_->getUserID())
            {
                currentUser_->setStatus(status);
                emit acceptCurrentUserStatus();
            }
            return;
        }
    }
}

void Client::addNewMessage(QString message)
{
    Message tempMessage;
    QString from = message.section(delimiter_, 1, 1);
    QString to = message.section(delimiter_, 2, 2);
    QString messageText = message.section(delimiter_, 3, 3);
    QString createTime = message.section(delimiter_, 4, 4);

    int fromNum = from.toInt();
    int toNum = to.toInt();

    tempMessage.setFrom(fromNum);
    tempMessage.setTo(toNum);
    tempMessage.setMessage(messageText);
    tempMessage.setCreateTime(createTime);
    emit addMessage(getNameByUserID(fromNum)
                    , getNameByUserID(toNum)
                    , messageText
                    , createTime);

    chatMessages_.push_back(tempMessage);
}

void Client::setOnlineStatus(bool isDisconnect)
{
    connectionStatus_ = !isDisconnect;    
    emit displayConnectionStatus();
}

void Client::clearData()
{
    currentUser_->setUserID(0);
    currentUser_->setName("");
    currentUser_->setStatus(0);
    chatUsers_.clear();
    chatMessages_.clear();
}

void Client::logoutClient()
{
    socket_->dataTransmission(QString::number(CommonFunc::Commands::clientLogout_));
}

void Client::showChatWindow()
{
    int currentUserID = currentUser_->getUserID();

    for(auto& message : chatMessages_)
    {
        if(isPrivateChat_ && message.getTo() == 0)
        {
            continue;
        }

        if(isSelectedUserOnly_)
        {
            if(isPrivateChat_)
            {                
                if(!((message.getFrom() == selectedUserID_ && message.getTo() == currentUserID) ||
                      (message.getFrom() == currentUserID && message.getTo() == selectedUserID_)))
                {
                    if(!((message.getFrom() == selectedUserID_) || (message.getTo() == selectedUserID_)))
                    {
                        continue;
                    }
                }
            }
            else
            {
                if(!((message.getFrom() == selectedUserID_)
                      ||(message.getTo() == selectedUserID_)))
                {
                    continue;
                }
            }
        }

        emit addItemToChatWindow(getNameByUserID(message.getFrom())
                                 , getNameByUserID(message.getTo())
                                 , message.getMessage()
                                 , message.getCreateTime());
    }
}

void Client::getConnectionConfig()
{
    QFile fileStream(connectionConfigFile_);
    QTextStream line(&fileStream);

    fileStream.open(QIODevice::ReadOnly);
    if (!fileStream.isOpen())
    {
        saveConnectionConfig();
        return;
    }

    while (!line.atEnd()) {
        QString parameter = line.readLine();

        if (parameter.section(':', 0, 0) == "serverIPAddress")
        {
            QString addressIP = parameter.section(':', 1, 1);
            if (CommonFunc::checkAddressIP(addressIP))
            {
                serverAddress_ = addressIP;
            }
        }
        else if (parameter.section(':', 0, 0) == "serverPort")
        {
            quint16 port = (parameter.section(':', 1, 1)).toUShort();
            if (CommonFunc::checkPort(port))
            {
                serverPort_ = port;
            }
        }
    }
    fileStream.close();
    saveConnectionConfig();
}

void Client::saveConnectionConfig()
{
    QFile fileStream(connectionConfigFile_);
    QTextStream stream(&fileStream);

    fileStream.open(QIODeviceBase::Truncate | QIODeviceBase::WriteOnly);
    if (!fileStream.isOpen())
    {
        return;
    }

    QString saveData = "serverIPAddress:" + serverAddress_ + "\n"
                       + "serverPort:" + QString::number(serverPort_) + "\n";
    stream << saveData;
    fileStream.close();
}

