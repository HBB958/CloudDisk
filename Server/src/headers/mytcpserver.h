#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer& getInstance();
    void incomingConnection(qintptr handle) override;
    QList<MyTcpSocket*> m_tcpSocketList;//用于存客户端的Socket指针
    void deleteSocket(MyTcpSocket* mysocket);
    void resend(char* strName,PDU* pdu);
    QThreadPool threadPool;
private:
    MyTcpServer();
    MyTcpServer(const MyTcpServer& instance) = delete;
    MyTcpServer& operator=(const MyTcpServer&) = delete;
};

#endif // MYTCPSERVER_H
