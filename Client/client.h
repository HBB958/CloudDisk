#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "reshandler.h"

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    ~Client();
    void loadConfig();
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootPath;
    QTcpSocket socket;
    static Client& getInstance();
    void recvMsg();
    QString m_strLoginName;
    void sendMsg(PDU* pdu);
    PDU* readMsg();
    void handMsg(PDU* pdu);
    ResHandler* rh;
    QByteArray buffer;

public slots:
    void showConnect();

private slots:
    //void on_pushButton_clicked();

    void on_regist_PB_clicked();

    void on_login_PB_clicked();

private:
    Ui::Client *ui;//获取ui界面控件
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client&) = delete;
};
#endif // CLIENT_H
