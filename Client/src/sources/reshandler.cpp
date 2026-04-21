#include "client.h"
#include "index.h"
#include "reshandler.h"


#include <QMessageBox>
#include<stdlib.h>

ResHandler::ResHandler()
{

}

void ResHandler::regist()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"提示","注册成功");
    }else{
        QMessageBox::warning(&Client::getInstance(),"提示","注册失败");
    }
}

void ResHandler::login()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    int num;
    memcpy(&num,pdu->caData+sizeof(bool),sizeof(num));
    qDebug()<<"num"<<num;
    if(ret&&num!=1){
        Index::getInstance().show();
        Index::getInstance().setWindowTitle(QString("首页(%1)").arg(Client::getInstance().m_strLoginName));
        Client::getInstance().hide();
    }
    else if(ret&&num==1){
        QMessageBox::warning(&Client::getInstance(),"提示","用户已在线");
    }
    else{
        QMessageBox::warning(&Client::getInstance(),"提示","用户名或密码错误");
    }
}

void ResHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret;
    memcpy(&ret,pdu->caData+32,sizeof(int));
    qDebug()<<"findUser ret"<<ret;
    if(ret==-1){
        QMessageBox::warning(&Index::getInstance(),"提示","查询失败");
    }else if(ret==2){
        QMessageBox::warning(&Index::getInstance(),"提示",QString("%1 不存在").arg(caName));
    }else if(ret==1){
        int ret = QMessageBox::information(&Index::getInstance(),"提示","用户在线","添加好友","取消");
        if(ret==0){
            QString strCurName = Client::getInstance().m_strLoginName;
            if(caName==strCurName){
                QMessageBox::information(&Index::getInstance(),"提示","不能添加自己为好友");
                return;
            }
            PDU* pdu = mkPDU();
            pdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
            memcpy(pdu->caData+32,caName,32);
            Client::getInstance().sendMsg(pdu);
        }
    }else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"提示",QString("%1不在线").arg(caName));
    }
}

void ResHandler::onlineUser()
{
    int iSize = pdu->uiMsgLen/32;
    QStringList res;
    char caName[32] = {'\0'};
    for(int i = 0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+(i*32),32);
        res.append(caName);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateUserList(res);
}

void ResHandler::addFriend()
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof(int));
    qDebug()<<"addFriend ret"<<ret;
    if(ret==-1){
        QMessageBox::warning(&Index::getInstance(),"提示","查询失败");
    }else if(ret==2){
        QMessageBox::warning(&Index::getInstance(),"提示","用户已经是好友");
    }else if(ret==0){
        QMessageBox::warning(&Index::getInstance(),"提示","用户不在线");
    }
}

void ResHandler::addFriendResend()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret = QMessageBox::question(&Index::getInstance(),"提示",QString("是否同意%1的添加好友请求:?").arg(caName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST;
    memcpy(respdu->caData,pdu->caData,64);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::addFriendAgree()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(&Client::getInstance(),"提示","添加好友失败");
    }
}

void ResHandler::flushFriend()
{
    int iSize = pdu->uiMsgLen/32;
    QStringList res;
    char caName[32] = {'\0'};
    for(int i = 0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+i*32,32);
        res.append(caName);
    }
    Index::getInstance().getFriend()->updateFriendList(res);
}

void ResHandler::delFriend()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","删除好友成功");
        PDU* pdu = mkPDU();
        pdu->uiType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
        memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
        Client::getInstance().sendMsg(pdu);
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","对方不是您的好友");
    }
}

void ResHandler::chat()
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] = {'\0'};
    memcpy(caChatName,pdu->caData,32);
    c->m_strChatName = caChatName;
    c->setWindowTitle(caChatName);
    c->addShow_TE(QString("%1:%2").arg(caChatName).arg(pdu->caMsg));
}

void ResHandler::chatRes()
{
    QMessageBox::information(&Index::getInstance(),"提示","当前用户已经不是好友,请刷新好友后重试");
}

void ResHandler::mkdir()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","创建文件夹成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","文件夹已存在");
    }
}

void ResHandler::flushFile()
{
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    QList<FileInfo *> pFileList;
    for(int i = 0;i<iCount;i++){
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo,pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));

        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updataFileList(pFileList);
}

void ResHandler::delFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","删除文件成功");
        PDU* pdu = mkPDU(Index::getInstance().getFile()->m_strCurPath.size()+1);
        pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
        memcpy(pdu->caMsg,Index::getInstance().getFile()->m_strCurPath.toStdString().c_str(),Index::getInstance().getFile()->m_strCurPath.toStdString().size());
        Client::getInstance().sendMsg(pdu);
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","删除文件失败");
    }
}

void ResHandler::delFileDir()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","删除文件夹成功");
        PDU* pdu = mkPDU(Index::getInstance().getFile()->m_strCurPath.size()+1);
        pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
        memcpy(pdu->caMsg,Index::getInstance().getFile()->m_strCurPath.toStdString().c_str(),Index::getInstance().getFile()->m_strCurPath.toStdString().size());
        Client::getInstance().sendMsg(pdu);
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","删除文件夹失败");
    }
}

void ResHandler::renameFile()
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret==1){
        QMessageBox::information(&Index::getInstance(),"提示","重命名成功");
        PDU* pdu = mkPDU(Index::getInstance().getFile()->m_strCurPath.size()+1);
        pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
        memcpy(pdu->caMsg,Index::getInstance().getFile()->m_strCurPath.toStdString().c_str(),Index::getInstance().getFile()->m_strCurPath.toStdString().size());
        Client::getInstance().sendMsg(pdu);
    }else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"提示","源文件不存在");
    }else if(ret==2){
        QMessageBox::information(&Index::getInstance(),"提示","新文件已存在");
    }
}

void ResHandler::moveFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","移动成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","移动失败");
    }
}

void ResHandler::uploadFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        Index::getInstance().getFile()->uploadFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","上传文件,服务器初始化失败");
    }
}

void ResHandler::uploadFileData()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","上传文件成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","上传文件失败");
    }
}

void ResHandler::downloadFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        qint64 size;
        memcpy(&size,pdu->caData+sizeof(ret),sizeof(size));
        Index::getInstance().getFile()->m_iDownloadFileSize = size;
        Index::getInstance().getFile()->flag = true;
        Index::getInstance().getFile()->downloadFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","下载文件,服务器初始化失败");
    }
}

void ResHandler::downloadFileData()
{
    Index::getInstance().getFile()->downloadFileData(pdu);
    if(Index::getInstance().getFile()->m_iDownloadFileSize==Index::getInstance().getFile()->m_iReceiveSize){
        QMessageBox::information(&Index::getInstance(),"提示","下载成功");
        Index::getInstance().getFile()->flag = false;
    }
}

void ResHandler::shareFile()
{
    QMessageBox::information(Index::getInstance().getFile(), "分享文件", "文件已分享");
}

void ResHandler::shareFileRequest()
{
    QString strSharePath = QString(pdu->caMsg);
    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size()-index-1);

    QString strMsg = QString("%1 分享文件：%2\n是否接收?").arg(pdu->caData).arg(strFileName);
    int ret = QMessageBox::question(&Index::getInstance(), "分享文件", strMsg);
    if (ret != QMessageBox::Yes) {
        return;
    }
    //caData存当前用户名，caMsg存分享文件的路径
    PDU* respdu = mkPDU(pdu->uiMsgLen);
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;
    QString strName = Client::getInstance().m_strLoginName;
    memcpy(respdu->caData, strName.toStdString().c_str(), 32);
    memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::shareFileRespond()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","接收文件成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(),"提示","接收文件失败");
    }
}
