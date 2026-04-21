#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QString>
#include <QTcpSocket>
#include <qfile.h>



class MsgHandler
{
public:
    MsgHandler();
    PDU* pdu;
    QFile m_fUploadFile;//上传的文件
    qint64 m_iUploadFileSize;//文件总大小
    qint64 m_iReceiveSize;//已经上传的大小
    QString m_strFilePath;//下载的文件路径
    QFile m_fDownloadFile;//下载的文件
    PDU* regist();
    PDU* login(QString &m_strLoginName);
    PDU* findUser();
    PDU* onlineUser();
    PDU* addFriend();
    PDU* addFriendAgree();
    PDU* flushFriend();
    PDU* DelFriend();
    PDU* chat();
    PDU* mkdir();
    PDU* flushFile();
    PDU* delFile();
    PDU* delFileDir();
    PDU* renameFile();
    PDU* moveFile();
    PDU* uploadFile();
    PDU* uploadFileData();
    PDU* downloadFile();
    PDU* downloadFileData(QTcpSocket *psocket);
    PDU* shareFile();
    PDU* shareFileAgree(PDU* pdu);
    bool copyDir(QString strSrcDir, QString strDestDir);

};

#endif // MSGHANDLER_H
