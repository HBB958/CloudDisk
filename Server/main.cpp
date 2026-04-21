#include "server.h"
#include "operatedb.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server::getInstance();
    OperateDB::getInstance().connect();
    return a.exec();
}
