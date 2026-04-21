#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:

    ~Server();
    void loadConfig();
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootPath;
    static Server& getInstance();

private:
    Ui::Server *ui;
    Server(QWidget *parent = nullptr);
    Server(const Server& instance) = delete;
    Server& operator=(const Server&) = delete;
};
#endif // SERVER_H
