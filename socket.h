#ifndef SOCKET_H
#define SOCKET_H

#include "commonfunc.h"

#include <QObject>
#include <QTcpSocket>
#include <QString>

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QString addressIP, quint16 port, QObject *parent = nullptr);
    ~Socket();

    bool connectionToTheServer(QString addressIP, quint16 port);
    void dataTransmission(QString sendData);

signals:
    void dataIsReady(QString data);
    void setDisconnectStatus(bool isDisconnect);

public slots:
    void disconnecting();
    void dataRecieving();
	
private:
    QByteArray buffer;
    QString serverAddress_;
    quint16 serverPort_{};
    QTcpSocket* socket_;
    char packetSeparator_{ '#' };
};

#endif // SOCKET_H
