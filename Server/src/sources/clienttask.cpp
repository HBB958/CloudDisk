#include "clienttask.h"

#include <QTcpSocket>
#include <qthread.h>

ClientTask::ClientTask(MyTcpSocket *socket):socket(socket)
{

}

void ClientTask::run()
{
    socket->moveToThread(QThread::currentThread());
    connect(socket,&QTcpSocket::readyRead,socket,&MyTcpSocket::recvMsg);
    connect(socket,&QTcpSocket::disconnected,socket,&MyTcpSocket::clientOffline);
}
