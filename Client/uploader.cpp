#include "client.h"
#include "protocol.h"
#include "uploader.h"

#include <QFile>
#include <QThread>

Uploader::Uploader(QString strUploadFilePath)
{
    m_strUploadFilePath = strUploadFilePath;
}

void Uploader::start()
{
    QThread* thread = new QThread;
    this->moveToThread(thread);//连接线程
    // 1. 线程启动时，触发上传操作
    connect(thread,&QThread::started,this,&Uploader::uploadFile);
    // 2. 上传完成时，结束线程的事件循环
    connect(this,&Uploader::finished,thread,&QThread::quit);
    // 3. 线程结束后，自动删除线程对象
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    thread->start();
}

void Uploader::uploadFile()
{
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        emit errorMsgBox("打开文件失败");
        emit finished();
        return;
    }
    while(true){
        PDU* pdu = mkPDU(4096);
        pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
        qint64 ret = file.read(pdu->caMsg,4096);
        if(ret==0){
            break;
        }
        if(ret<0){
            emit errorMsgBox("读取文件失败");
            break;
        }
        pdu->uiMsgLen = ret;
        pdu->uiPDULen = ret+sizeof(PDU);
        emit uploadPDU(pdu);

    }
    file.close();
    emit finished();
}

