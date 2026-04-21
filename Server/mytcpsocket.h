#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <qtcpsocket.h>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    QString m_strLoginName;
    void sendMsg(PDU* pdu);
    PDU* readMsg();
    PDU* handleMsg(PDU* pdu);
    MsgHandler* mh;
    QByteArray buffer;
public slots:
    void recvMsg();
    void clientOffline();
};

#endif // MYTCPSOCKET_H
