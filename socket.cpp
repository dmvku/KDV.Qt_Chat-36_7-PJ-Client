#include "socket.h"

Socket::Socket(QString addressIP, quint16 port, QObject *parent)
    : QObject{ parent }, serverAddress_ (addressIP), serverPort_(port)
{
    socket_ = new QTcpSocket();

    connect(socket_, &QTcpSocket::disconnected, this, &Socket::disconnecting);
    connect(socket_, &QTcpSocket::readyRead, this, &Socket::dataRecieving);
}

Socket::~Socket()
{
    socket_->close();
    delete socket_;
}

bool Socket::connectionToTheServer(QString addressIP, quint16 port)
{    
    socket_->connectToHost(addressIP, port);
    if(!socket_->waitForConnected(10000))
    {
        disconnecting();
        return false;
    }

    serverAddress_ = addressIP;
    serverPort_ = port;    
    return true;
}

void Socket::disconnecting()
{
    socket_->disconnectFromHost();    
    emit setDisconnectStatus(true);
}

void Socket::dataTransmission(QString sendData)
{
    socket_->waitForBytesWritten();
    socket_->write((sendData  + packetSeparator_).toUtf8());
}

void Socket::dataRecieving()
{
    buffer.append(socket_->readAll());

    int index;
    while ((index = buffer.indexOf(packetSeparator_)) != -1)
    {
        QByteArray packet = buffer.left(index);
        buffer = buffer.mid(index + 1);

        emit dataIsReady(QString::fromUtf8(packet));
    }
}
