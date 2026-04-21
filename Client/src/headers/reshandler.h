#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"



class ResHandler
{
public:
    ResHandler();
    PDU* pdu;
    void regist();
    void login();
    void findUser();
    void onlineUser();
    void addFriend();
    void addFriendResend();
    void addFriendAgree();
    void flushFriend();
    void delFriend();
    void chat();
    void chatRes();
    void mkdir();
    void flushFile();
    void delFile();
    void delFileDir();
    void renameFile();
    void moveFile();
    void uploadFile();
    void uploadFileData();
    void downloadFile();
    void downloadFileData();
    void shareFile();
    void shareFileRequest();
    void shareFileRespond();
};

#endif // RESHANDLER_H
