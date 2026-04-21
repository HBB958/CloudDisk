#include "msghandler.h"
#include "mytcpserver.h"
//#include "mytcpsocket.h"
#include "operatedb.h"
#include "server.h"
#include <QDebug>
#include <QDir>
#include<stdlib.h>

MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::regist()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd,pdu->caData+32,32);

    bool ret = OperateDB::getInstance().handeRegist(caName,caPwd);
    qDebug()<<"ret"<<ret;
    if(ret){
        QDir dir;
        bool mkRet = dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
        qDebug()<<"mkRet"<<mkRet;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_REGIST_RESPOND;
    return respdu;
}

PDU *MsgHandler::login(QString &m_strLoginName)
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd,pdu->caData+32,32);

    int num = OperateDB::getInstance().handeFindUser(caName);
    bool ret = OperateDB::getInstance().handeLogin(caName,caPwd);

    qDebug()<<" login ret"<<ret;
    if(ret&&num!=1){
        m_strLoginName = caName;
    }
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(ret));
    memcpy(respdu->caData+sizeof(ret),&num,sizeof(int));
    return respdu;
}

PDU *MsgHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret = OperateDB::getInstance().handeFindUser(caName);
    qDebug()<<"findUser ret"<<ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,caName,32);
    memcpy(respdu->caData+32,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList res = OperateDB::getInstance().handeOnlineUser();
    qDebug()<<"onlineUser res"<<res.size();
    PDU* respdu = mkPDU(res.size()*32);
    for(int i = 0;i<res.size();i++){
        memcpy(respdu->caMsg+(i*32),res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::addFriend()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    char caTarName[32] = {'\0'};
    memcpy(caTarName,pdu->caData+32,32);

    int ret = OperateDB::getInstance().handeAddFriend(caCurName,caTarName);
    qDebug()<< "addFriend ret"<<ret;
    if(ret==1){
        MyTcpServer::getInstance().resend(caTarName,pdu);
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    return respdu;

}

PDU *MsgHandler::addFriendAgree()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    char caTarName[32] = {'\0'};
    memcpy(caTarName,pdu->caData+32,32);

    bool ret = OperateDB::getInstance().handeAddFriendAgree(caCurName,caTarName);
    qDebug()<< "addFriendAgree ret"<<ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND;
    MyTcpServer::getInstance().resend(caCurName,respdu);
    return respdu;
}

PDU *MsgHandler::flushFriend()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);

    QStringList res = OperateDB::getInstance().handeOnlineFriend(caName);
    PDU* respdu = mkPDU(res.size()*32);
    for(int i = 0;i<res.size();i++){
        memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::DelFriend()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    char delName[32] = {'\0'};
    memcpy(delName,pdu->caData+32,32);

    bool ret = OperateDB::getInstance().handeDelFriend(caName,delName);
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::chat()
{
    char caTarName[32] = {'\0'};
    memcpy(caTarName,pdu->caData+32,32);
    char caCurName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    bool ret = OperateDB::getInstance().isFriend(caCurName,caTarName);
    if(!ret){
        PDU* respdu = mkPDU();
        memcpy(respdu->caData,&ret,sizeof(bool));
        respdu->uiType = ENUM_MSG_TYPE_CHAT_RESPOND;
        return respdu;
    }else{
        MyTcpServer::getInstance().resend(caTarName,pdu);
        return NULL;
    }
}

PDU *MsgHandler::mkdir()
{
    QDir dir;
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    bool ret;
    if(dir.exists(strPath)){
        ret = false;
    }
    ret = dir.mkdir(QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData));
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_MKDIR_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::flushFile()
{
    QDir dir(pdu->caMsg);
    QFileInfoList fileInfoList = dir.entryInfoList();
    int iFileCount = fileInfoList.size();
    PDU* respdu = mkPDU((iFileCount-2)*sizeof(FileInfo));
    respdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    for(int i = 0,j=0;i<iFileCount;i++){
        if(fileInfoList[i].fileName()=="."||fileInfoList[i].fileName()==".."){
            continue;
        }
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg+(j++);
        memcpy(pFileInfo->caName,fileInfoList[i].fileName().toStdString().c_str(),32);
        if(fileInfoList[i].isDir()){
            pFileInfo->iType = 0;
        }else{
            pFileInfo->iType = 1;
        }
    }
    return respdu;
}

PDU *MsgHandler::delFile()
{
    QString UserPath = QString("%1").arg(pdu->caMsg);
    QFile fil;
    bool ret;
    if(!fil.exists(UserPath)){
        qDebug()<<"文件不存在";
        ret = false;
    }
    ret = fil.remove(UserPath);
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::delFileDir()
{
    QString UserPath = QString("%1").arg(pdu->caMsg);
    qDebug()<<"111"<<UserPath;
    QDir dir(UserPath);
    bool ret;
    if(!dir.exists()){
        ret = false;
    }
    ret = dir.removeRecursively();
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_DEL_FILE_DIR_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::renameFile()
{
    qDebug()<<"111";
    char oldName[32] = {'\0'};
    memcpy(oldName,pdu->caData,32);
    char newName[32] = {'\0'};
    memcpy(newName,pdu->caData+32,32);
    QString Path = QString("%1").arg(pdu->caMsg);
    QString oldPath = QString("%1/%2").arg(Path).arg(oldName);
    QString newPath = QString("%1/%2").arg(Path).arg(newName);
    QFile oldqf(oldPath);
    QFile newqf(newPath);
    int ret;
    if(!oldqf.exists()){
        qDebug()<<"源文件不存在";
        ret=0;
    }
    if(newqf.exists()){
        qDebug()<<"新文件已存在";
        ret=2;
    }
    bool flag = oldqf.rename(newPath);
    if(flag){
        qDebug()<<"修改成功";
        ret=1;
    }
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_RENAME_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(ret));
    return respdu;
}

PDU *MsgHandler::moveFile()
{
    int SrcLen = 0;
    memcpy(&SrcLen,pdu->caData,sizeof(int));
    int TarLen = 0;
    memcpy(&TarLen,pdu->caData+sizeof(int),sizeof(int));

    char* caSrcPath = new char[SrcLen+1];
    char* caTarPath = new char[TarLen+1];
    memset(caSrcPath,'\0',SrcLen+1);
    memset(caTarPath,'\0',TarLen+1);
    memcpy(caSrcPath,pdu->caMsg,SrcLen);
    memcpy(caTarPath,pdu->caMsg+SrcLen,TarLen);

    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
    bool ret = QFile::rename(caSrcPath,caTarPath);
    memcpy(respdu->caData,&ret,sizeof(bool));
    delete[] caSrcPath;
    delete[] caTarPath;
    caSrcPath = NULL;
    caTarPath = NULL;
    return respdu;
}

PDU *MsgHandler::uploadFile()
{
    m_iReceiveSize = 0;
    char caFileName[32] = {'\0'};
    memcpy(caFileName,pdu->caData,32);
    memcpy(&m_iUploadFileSize,pdu->caData+32,sizeof(qint64));
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);

    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(ret));
    return respdu;
}

PDU *MsgHandler::uploadFileData()
{
    m_fUploadFile.write(pdu->caMsg,pdu->uiMsgLen);
    m_iReceiveSize+=pdu->uiMsgLen;
    if(m_iReceiveSize<m_iUploadFileSize){
        return NULL;
    }
    m_fUploadFile.close();
    bool ret = m_iReceiveSize==m_iUploadFileSize;

    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(ret));
    return respdu;
}

PDU *MsgHandler::downloadFile()
{
    char FileName[32] = {'\0'};
    memcpy(&FileName,pdu->caData,32);
    m_strFilePath = QString("%1/%2").arg(pdu->caMsg).arg(FileName);
    qDebug()<<"Path"<<m_strFilePath;
    m_fDownloadFile.setFileName(m_strFilePath);
    bool ret= m_fDownloadFile.open(QIODevice::ReadOnly);
    qint64 size = m_fDownloadFile.size();

    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(ret));
    memcpy(respdu->caData+sizeof(ret),&size,sizeof(size));
    return respdu;
}

PDU *MsgHandler::downloadFileData(QTcpSocket *psocket)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(ret));
    if(ret){
        while(true){
            PDU* respdu = mkPDU(4096);
            respdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPOND;
            qint64 ret = m_fDownloadFile.read(respdu->caMsg,4096);
            if(ret==0){
                break;
            }
            if(ret<0){
                break;
            }
            respdu->uiMsgLen = ret;
            respdu->uiPDULen = ret+sizeof(PDU);
            psocket->write((char*)respdu,respdu->uiPDULen);
        }
    }
    m_fDownloadFile.close();
    return NULL;
}

PDU *MsgHandler::shareFile()
{
    char strCurName[32] = {'\0'};
    int friend_num = 0;
    memcpy(strCurName, pdu->caData, 32);
    memcpy(&friend_num, pdu->caData+32, sizeof(int));
    int size = friend_num*32;
    //转发的pdu存发送者名字和文件路径
    PDU* resendpdu = mkPDU(pdu->uiMsgLen-size);//减去接收者名字的空间，只留路径
    resendpdu->uiType = pdu->uiType;
    memcpy(resendpdu->caData, strCurName, 32);
    memcpy(resendpdu->caMsg, pdu->caMsg+size, pdu->uiMsgLen-size);//pdu中路径在接收者名字后面
    qDebug() << "shareFile friend_num " << friend_num;
    //转发给每个接收者
    char caRecvName[32] = {'\0'};
    for(int i=0; i<friend_num; i++) {
        memcpy(caRecvName, pdu->caMsg+i*32, 32);
        qDebug() << "caRecvName" << caRecvName;
        MyTcpServer::getInstance().resend(caRecvName, resendpdu);
    }
    free(resendpdu);
    resendpdu = NULL;
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::shareFileAgree(PDU *pdu)
{
    QString strRecvPath = QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(pdu->caData);
    QString strShareFilePath = pdu->caMsg;
    int index = strShareFilePath.lastIndexOf('/');
    QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
    strRecvPath = strRecvPath + '/' + strFileName;
    QFileInfo fileInfo(strShareFilePath);
    qDebug() << "strShareFilePath" << strShareFilePath
             << "strRecvPath" << strRecvPath;
    bool ret = true;
    if (fileInfo.isFile()) {
        ret = QFile::copy(strShareFilePath, strRecvPath);
        qDebug() << "shareFileAgree ret: " << ret;
    } else if (fileInfo.isDir()) {
        ret = copyDir(strShareFilePath, strRecvPath);
    }
    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND;
    return respdu;

}

bool MsgHandler::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();
    bool ret = true;
    QString srcTmp;
    QString destTmp;
    for (int i=0; i<fileInfoList.size(); i++) {
        if (fileInfoList[i].isFile()) {
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            if (!QFile::copy(srcTmp, destTmp)) {
                ret = false;
            }
        } else if (fileInfoList[i].isDir()) {
            if (fileInfoList[i].fileName() == QString(".") ||
                    fileInfoList[i].fileName() == QString("..")) {
                continue;
            }
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDestDir + '/' + fileInfoList[i].fileName();
            if (!copyDir(srcTmp, destTmp)) {
                 ret = false;
            }
        }
    }
    return ret;
}








