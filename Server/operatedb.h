#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDB : public QObject
{
    Q_OBJECT
public:

    QSqlDatabase m_db;
    ~OperateDB();
    void connect();
    static OperateDB& getInstance();
    bool handeRegist(const char* caName,const char* caPwd);
    bool handeLogin(const char* caName,const char* caPwd);
    void handeOffine(const char* caName);
    int handeFindUser(const char* caName);
    QStringList handeOnlineUser();
    bool isFriend(const char* caCurName,const char* caTarName);
    int handeAddFriend(const char* caCurName,const char* caTarName);
    bool handeAddFriendAgree(const char* caCurName,const char* caTarName);
    QStringList handeOnlineFriend(const char* caName);
    bool handeDelFriend(const char* caCurName,const char* caDelName);

private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator=(const OperateDB&) = delete;

signals:

};

#endif // OPERATEDB_H
