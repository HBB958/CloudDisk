#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"

#include <QFile>
#include<QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <qmessagebox.h>


Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    socket.connectToHost(QHostAddress(m_strIP),m_usPort);
    connect(&socket,&QTcpSocket::connected,this,&Client::showConnect);
    connect(&socket,&QTcpSocket::readyRead,this,&Client::recvMsg);
    rh = new ResHandler;
}

Client::~Client()
{
    delete ui;
    delete rh;
}

//获取IP和端口号
void Client::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QString strData = QString(file.readAll());
        qDebug()<<"strData"<<strData;
        QStringList strList = strData.split("\r\n");
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        m_strRootPath = strList[2];
        qDebug()<<"loadconfig ip"<<m_strIP<<"port"<<m_usPort<<"m_strRootPath"<<m_strRootPath;
        file.close();
    }else{
        qDebug()<<"loadConfig打开配置文件失败";
    }
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::recvMsg()
{
    //处理粘包问题
    QByteArray data = socket.readAll();
    buffer.append(data);
    while (buffer.size() >= int(sizeof(PDU))) {
        PDU* pdu = (PDU*)buffer.data();
        if (buffer.size() < int(pdu->uiPDULen)) {
            break;
        }
        handMsg(pdu);
        buffer.remove(0, pdu->uiPDULen);
    }
}

void Client::sendMsg(PDU *pdu)
{
    if(pdu==NULL){
        return;
    }
    socket.write((char*)pdu,pdu->uiPDULen);
    qDebug() <<"send msg pdu->uiPDULen"<<pdu->uiPDULen
             <<"pdu->uiMsgLen"<<pdu->uiMsgLen
             <<"pdu->uiType"<<pdu->uiType
             <<"pdu->caData"<<pdu->caData
             <<"pdu->caData+32"<<pdu->caData+32
             <<"pdu->uiMsg"<<pdu->caMsg;
    free(pdu);
    pdu=NULL;
}

//PDU *Client::readMsg()
//{
//    uint uiPDULen = 0;
//    socket.read((char*)&uiPDULen,sizeof(uint));
//    uint uiMsgLen = uiPDULen-sizeof(PDU);
//    PDU* pdu = mkPDU(uiMsgLen);
//    socket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
//    qDebug() <<"send msg pdu->uiPDULen"<<pdu->uiPDULen
//             <<"pdu->uiMsgLen"<<pdu->uiMsgLen
//             <<"pdu->uiType"<<pdu->uiType
//             <<"pdu->caData"<<pdu->caData
//             <<"pdu->caData+32"<<pdu->caData+32
//             <<"pdu->uiMsg"<<pdu->caMsg;
//    return pdu;
//}

void Client::handMsg(PDU *pdu)
{
    if(pdu==NULL){
        return;
    }
    rh->pdu = pdu;
    switch(pdu->uiType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        rh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        rh->login();
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPOND:{
        rh->findUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:{
        rh->onlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        rh->addFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        rh->addFriendResend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND:{
        rh->addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        rh->flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:{
        rh->delFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        rh->chat();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_RESPOND:{
        rh->chatRes();
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPOND:{
        rh->mkdir();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        rh->flushFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_RESPOND:{
        rh->delFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_DIR_RESPOND:{
        rh->delFileDir();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_RESPOND:{
        rh->renameFile();
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:{
        rh->moveFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
        rh->uploadFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:{
        rh->uploadFileData();
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:{
        rh->downloadFile();
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPOND:{
        rh->downloadFileData();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        rh->shareFileRequest();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        rh->shareFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND:{
        rh->shareFileRespond();
        break;
    }
    default:
        break;
    }
}

void Client::showConnect()
{
    qDebug()<<"连接服务器成功";
}


void Client::on_regist_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty() || strName.toStdString().size()>32){
        QMessageBox::warning(this,"提示","用户名长度非法");
        return;
    }
    if(strPwd.isEmpty() || strPwd.toStdString().size()>32){
        QMessageBox::warning(this,"提示","用户名长度非法");
        return;
    }
    PDU* pdu = mkPDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_REGIST_REQUEST;
    sendMsg(pdu);
}

void Client::on_login_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty() || strName.toStdString().size()>32){
        QMessageBox::warning(this,"提示","用户名长度非法");
        return;
    }
    if(strPwd.isEmpty() || strPwd.toStdString().size()>32){
        QMessageBox::warning(this,"提示","密码长度非法");
        return;
    }
    m_strLoginName = strName;
    PDU* pdu = mkPDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    pdu->uiType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    sendMsg(pdu);
}
