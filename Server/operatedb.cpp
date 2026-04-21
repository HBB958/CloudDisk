#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

OperateDB::~OperateDB()
{
    m_db.close();
}

void OperateDB::connect()
{
    m_db.setHostName("localhost");
    m_db.setPort(3307);
    m_db.setDatabaseName("qtmydb1011");
    m_db.setUserName("root");
    m_db.setPassword("123456");
    if(m_db.open()){
        qDebug()<<"数据库连接成功";
    }else{
        qDebug()<<"数据库连接失败" << m_db.lastError().text();
    }
}

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

bool OperateDB::handeRegist(const char *caName, const char *caPwd)
{
    if(caName == NULL || caPwd == NULL){
        return false;
    }
    QString sql = QString("select * from user_info where name='%1'").arg(caName);
    qDebug() << "查询用户是否存在"<<sql;
    QSqlQuery q;
    if(!q.exec(sql)){
        qDebug()<<"执行失败";
        return false;
    }
    if(q.next()){
        qDebug()<<"用户已存在";
        return false;
    }
    sql = QString("insert into user_info(name,pwd) values('%1','%2')").arg(caName).arg(caPwd);
    qDebug() << "插入用户"<<sql;
    return q.exec(sql);
}

bool OperateDB::handeLogin(const char *caName, const char *caPwd)
{
    if(caName==NULL || caPwd == NULL){
        return false;
    }
    QString sql = QString("select * from user_info where name = '%1' and pwd = '%2'").arg(caName).arg(caPwd);
    qDebug() << "查询用户和密码是否存在";
    QSqlQuery q;
    if(!q.exec(sql)){
        qDebug()<<"运行失败";
        return false;
    }
    if(!q.next()){
        qDebug() << "用户不存在";
        return false;
    }
    sql = QString("update user_info set online = 1 where name = '%1' and pwd = '%2'").arg(caName).arg(caPwd);
    qDebug()<<"更新用户在线状态,用户上线";
    return q.exec(sql);
}

void OperateDB::handeOffine(const char *caName)
{
    if(caName==NULL){
        return;
    }
    QString sql = QString("update user_info set online = 0 where name = '%1'").arg(caName);
    QSqlQuery q;
    qDebug()<<"更新用户在线状态,用户下线";
    q.exec(sql);
}

int OperateDB::handeFindUser(const char *caName)
{
    if(caName == NULL){
        return -1;
    }
    QString sql = QString("select online from user_info where name = '%1'").arg(caName);
    QSqlQuery q;
    qDebug()<<"通过用户名查找online字段";
    q.exec(sql);
    if(q.next()){
        return q.value(0).toInt();
    }
    return 2;
}

QStringList OperateDB::handeOnlineUser()
{
    QString sql = QString("select name from user_info where online=1");
    QSqlQuery q;
    q.exec(sql);
    QStringList res;
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

bool OperateDB::isFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName==NULL||caTarName==NULL){
        return -1;
    }
    QString sql = QString(R"(
                  select * from friend
                  where
                  (
                    user_id=(select id from user_info where name='%1')
                    and
                    friend_id=(select id from user_info where name='%2')
                  )
                  or
                  (
                    user_id=(select id from user_info where name='%2')
                    and
                    friend_id=(select id from user_info where name='%1')
                  );
                  )").arg(caCurName).arg(caTarName);
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return true;
    }
    return false;
}

int OperateDB::handeAddFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName==NULL||caTarName==NULL){
        return -1;
    }
    QString sql = QString(R"(
                  select * from friend
                  where
                  (
                    user_id=(select id from user_info where name='%1')
                    and
                    friend_id=(select id from user_info where name='%2')
                  )
                  or
                  (
                    user_id=(select id from user_info where name='%2')
                    and
                    friend_id=(select id from user_info where name='%1')
                  );
                  )").arg(caCurName).arg(caTarName);
    qDebug()<<"查询是否已经添加好友"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return 2;
    }
    sql = QString("select online from user_info where name = '%1'").arg(caTarName);
    q.exec(sql);
    if(q.next()){
        return q.value(0).toInt();
    }
    return -1;
}

bool OperateDB::handeAddFriendAgree(const char *caCurName, const char *caTarName)
{
    if(caTarName==NULL||caCurName==NULL){
        return false;
    }
    QString sql = QString(R"(
                          insert into friend(user_id,friend_id)
                          select u1.id,u2.id
                          from user_info u1,user_info u2
                          where u1.name='%1' and u2.name='%2'
                          )").arg(caCurName).arg(caTarName);
    QSqlQuery q;
    qDebug()<<"添加好友记录"<<sql;
    return(q.exec(sql));
}

QStringList OperateDB::handeOnlineFriend(const char *caName)
{
    QStringList res;
    QString sql = QString(R"(
                          select name from user_info
                          where id in
                          (
                            select user_id from friend where
                            friend_id=(select id from user_info where name='%1')
                            union
                            select friend_id from friend where
                            user_id=(select id from user_info where name='%1')
                          )
                          and online=1;)").arg(caName);
    QSqlQuery q;
    q.exec(sql);
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;

}

bool OperateDB::handeDelFriend(const char *caCurName, const char *caDelName)
{
    if(caCurName==NULL||caDelName==NULL){
        return false;
    }
    QString sql = QString(R"(
                  select * from friend
                  where
                  (
                    user_id=(select id from user_info where name='%1')
                    and
                    friend_id=(select id from user_info where name='%2')
                  )
                  or
                  (
                    user_id=(select id from user_info where name='%2')
                    and
                    friend_id=(select id from user_info where name='%1')
                  );
                      )").arg(caCurName).arg(caDelName);
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return false;
    }
    sql = QString(R"(
                  delete from friend
                  where
                  (
                    user_id=(select id from user_info where name='%1')
                    and
                    friend_id=(select id from user_info where name='%2')
                  )
                  or
                  (
                    user_id=(select id from user_info where name='%2')
                    and
                    friend_id=(select id from user_info where name='%1')
                  );
                      )").arg(caCurName).arg(caDelName);
    return q.exec(sql);
}
