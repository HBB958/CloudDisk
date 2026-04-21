#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
    connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::clientOffline);
    mh = new MsgHandler;
}

MyTcpSocket::~MyTcpSocket()
{
    delete mh;
}

void MyTcpSocket::sendMsg(PDU *pdu)
{
    if(pdu==NULL){
        return;
    }
    this->write((char*)pdu,pdu->uiPDULen);
    qDebug() <<"send msg pdu->uiPDULen"<<pdu->uiPDULen
             <<"pdu->uiMsgLen"<<pdu->uiMsgLen
             <<"pdu->uiType"<<pdu->uiType
             <<"pdu->caData"<<pdu->caData
             <<"pdu->caData+32"<<pdu->caData+32
             <<"pdu->uiMsg"<<pdu->caMsg;
    free(pdu);
    pdu = NULL;
}

//PDU *MyTcpSocket::readMsg()
//{
//    qDebug()<<"readMsg 接收消息长度" << this->bytesAvailable();
//    uint uiPDULen = 0;
//    this->read((char*)&uiPDULen,sizeof(uint));
//    uint uiMsgLen = uiPDULen-sizeof(PDU);
//    PDU* pdu = mkPDU(uiMsgLen);
//    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
//    qDebug() <<"send readMsg pdu->uiPDULen"<<pdu->uiPDULen
//             <<"pdu->uiMsgLen"<<pdu->uiMsgLen
//             <<"pdu->uiType"<<pdu->uiType
//             <<"pdu->caData"<<pdu->caData
//             <<"pdu->caData+32"<<pdu->caData+32
//             <<"pdu->uiMsg"<<pdu->caMsg;
//    return pdu;
//}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    PDU* respdu = NULL;
    if(pdu==NULL){
        return NULL;
    }
    mh->pdu = pdu;
    switch(pdu->uiType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        respdu = mh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        respdu = mh->login(m_strLoginName);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_REQUEST:{
        respdu = mh->findUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:{
        respdu = mh->onlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        respdu = mh->addFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST:{
        respdu = mh->addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
        respdu = mh->flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{
        respdu = mh->DelFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        respdu = mh->chat();
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_REQUEST:{
        respdu = mh->mkdir();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:{
        respdu = mh->flushFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_REQUEST:{
        respdu = mh->delFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_DIR_REQUEST:{
        respdu = mh->delFileDir();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_REQUEST:{
        respdu = mh->renameFile();
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:{
        respdu = mh->moveFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{
        respdu = mh->uploadFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:{
        respdu = mh->uploadFileData();
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
        respdu = mh->downloadFile();
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST:{
        respdu = mh->downloadFileData(this);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        respdu = mh->shareFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:{
        respdu = mh->shareFileAgree(pdu);
        break;
    }

    default:
        break;
    }
    return respdu;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "recvMsg 接收消息长度" << this->bytesAvailable();
    QByteArray data = this->readAll();
    buffer.append(data);
    while (buffer.size() >= int(sizeof(PDU))) {
        PDU* pdu = (PDU*)buffer.data();
        if (buffer.size() < int(pdu->uiPDULen)) {
            break;
        }
        PDU* respdu = handleMsg(pdu);
        sendMsg(respdu);
        buffer.remove(0, pdu->uiPDULen);
    }
}

void MyTcpSocket::clientOffline()
{
    OperateDB::getInstance().handeOffine(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().deleteSocket(this);
}
