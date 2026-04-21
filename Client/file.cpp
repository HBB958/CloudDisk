#include "client.h"
#include "file.h"
#include "index.h"
#include "ui_file.h"
#include "uploader.h"

#include <QFileDialog>
#include <qinputdialog.h>
#include <qmessagebox.h>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strUserPath;
    flushFile();
    m_pShareFile = new ShareFile;

}

File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::flushFile()
{
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::updataFileList(QList<FileInfo *> pFileList)
{
    ui->listWidget->clear();
    foreach(FileInfo* pFileInfo,m_pFileList){
        delete pFileInfo;
    }
    m_pFileList = pFileList;
    foreach(FileInfo* pFileInfo,pFileList){
        QListWidgetItem* pItem = new QListWidgetItem;
        if(pFileInfo->iType == 1){
            pItem->setIcon(QIcon(QPixmap(":/780 (1).png")));
        }else if(pFileInfo->iType==0){
            pItem->setIcon(QIcon(QPixmap(":/OIP-C2.png")));
        }
        pItem->setText(pFileInfo->caName);
        ui->listWidget->addItem(pItem);
    }
}

void File::uploadFile()
{
    Uploader* uploader = new Uploader(m_strUploadFilePath);
    connect(uploader,&Uploader::errorMsgBox,this,&File::uploadError);
    connect(uploader, &Uploader::uploadPDU, &Client::getInstance(), &Client::sendMsg);
    uploader->start();
}

void File::downloadFile()
{
    m_iReceiveSize = 0;
    m_fDownloadFile.setFileName(m_strFilePath);
    bool ret = m_fDownloadFile.open(QIODevice::WriteOnly);

    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
    memcpy(pdu->caData,&ret,sizeof(ret));
    Client::getInstance().sendMsg(pdu);
}

void File::downloadFileData(PDU *pdu)
{
    m_fDownloadFile.write(pdu->caMsg, pdu->uiMsgLen);
    m_iReceiveSize += pdu->uiMsgLen;
    if (m_iReceiveSize < m_iDownloadFileSize) {
        return;
    }
    m_fDownloadFile.close();
}

void File::on_mkDir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this,"提示","请输入文件夹名");
    if(strNewDir.toStdString().size()==0){
        return;
    }
    if(strNewDir.toStdString().size()>32){
        QMessageBox::information(this,"提示","文件夹名长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caData,strNewDir.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_delFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择一个文件");
        return;
    }
    foreach(FileInfo* pFileInfo,m_pFileList){
        if(pItem->text()==pFileInfo->caName&&pFileInfo->iType!=1){
            QMessageBox::information(this,"提示","删除文件夹请点击删除文件夹按钮");
            return;
        }
    }
    QString fileDelName = pItem->text();
    int ret = QMessageBox::question(this,"提示",QString("是否确认删除文件 %1").arg(fileDelName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    QString UserPath = m_strCurPath+"/"+fileDelName;
    PDU* pdu = mkPDU(UserPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
    memcpy(pdu->caMsg,UserPath.toStdString().c_str(),UserPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_delDir_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择一个文件夹");
        return;
    }
    foreach(FileInfo* pFileInfo,m_pFileList){
        if(pItem->text()==pFileInfo->caName&&pFileInfo->iType!=0){
            QMessageBox::information(this,"提示","删除文件请点击删除文件按钮");
            return;
        }
    }
    QString fileDelName = pItem->text();
    int ret = QMessageBox::question(this,"提示",QString("是否确认删除文件夹 %1").arg(fileDelName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    QString UserPath = m_strCurPath+"/"+fileDelName;
    PDU* pdu = mkPDU(UserPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_DEL_FILE_DIR_REQUEST;
    memcpy(pdu->caMsg,UserPath.toStdString().c_str(),UserPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_renameFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择一个文件或文件夹");
        return;
    }
    QString fileOldName = pItem->text();
    bool ok;
    QString fileNewName = QInputDialog::getText(this,"提示","请输入新的文件或文件夹名(需包含文件后缀名)",QLineEdit::Normal,"",&ok);
    if(!ok){
        return;
    }
    if(fileNewName.isEmpty()){
        QMessageBox::information(this,"提示","文件名不能为空");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_RENAME_REQUEST;
    memcpy(pdu->caData,fileOldName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,fileNewName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    foreach(FileInfo* pFileInfo,m_pFileList){
        if(item->text()==pFileInfo->caName&&pFileInfo->iType!=0){
            QMessageBox::information(this,"提示","请选择一个文件夹");
            return;
        }
    }
    m_strCurPath = m_strCurPath+"/"+item->text();
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath==m_strUserPath){
        QMessageBox::information(this,"提示","已经是最后一级文件夹");
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index,m_strCurPath.size()-index);
    flushFile();
}

void File::on_mvFile_PB_clicked()
{
    //判断按钮上的文字,是移动文件还是确认/取消
    if(ui->mvFile_PB->text()=="移动文件"){
        QListWidgetItem* pItem = ui->listWidget->currentItem();
        if(pItem==NULL){
            QMessageBox::information(this,"提示","请选择一个文件");
            return;
        }
        QMessageBox::information(this,"移动文件","请选择要移动到的目录");
        ui->mvFile_PB->setText("确认/取消");

        //记录选择的文件名和他的当前路径
        m_strMvName = pItem->text();//即将移动的文件名
        m_strMvPath = m_strCurPath;//当前路径
        return;
    }

    //判断是 确认/取消 时
    ui->mvFile_PB->setText("移动文件");
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    QString strTarPath;
    if(pItem==NULL){
        strTarPath = m_strCurPath;//如果选的不是文件夹,就移动到当前路径
    }else{
        strTarPath = m_strCurPath+"/"+pItem->text();//要移动到的路径
        foreach(FileInfo* pFileInfo,m_pFileList){
            //判断是不是文件夹
            if(pItem->text()==pFileInfo->caName&&pFileInfo->iType!=0){
                strTarPath = m_strCurPath;
                break;
            }
        }
    }
    //是否确认移动
    int ret = QMessageBox::information(this,"移动文件",QString("是否确认移动到\n%1").arg(strTarPath),"确认","取消");
    if(ret!=0){        
        return;
    }
    QString strSrcPath = m_strMvPath+"/"+m_strMvName;//旧路径
    strTarPath = strTarPath+"/"+m_strMvName;//新路径

    int iSrcPathLen = strSrcPath.toStdString().size();
    int iTarPathLen = strTarPath.toStdString().size();
    PDU* pdu = mkPDU(iSrcPathLen+iTarPathLen+1);
    pdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    memcpy(pdu->caData,&iSrcPathLen,sizeof(int));
    memcpy(pdu->caData+sizeof(int),&iTarPathLen,sizeof(int));
    memcpy(pdu->caMsg,strSrcPath.toStdString().c_str(),iSrcPathLen);
    memcpy(pdu->caMsg+iSrcPathLen,strTarPath.toStdString().c_str(),iTarPathLen);
    Client::getInstance().sendMsg(pdu);
}

void File::on_uploadFile_PB_clicked()
{
    m_strUploadFilePath = QFileDialog::getOpenFileName();
    if(m_strUploadFilePath==NULL){
        return;
    }
    qDebug()<<"m_strUploadFilePath"<<m_strUploadFilePath;
    int index = m_strUploadFilePath.lastIndexOf("/");
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
    qDebug()<<"strFileName"<<strFileName;
    if(strFileName.toStdString().size()>32){
        QMessageBox::information(this,"提示","文件名过长");
        return;
    }
    QFile file(m_strUploadFilePath);
    qint64 iFileSize = file.size();
    qDebug()<<"size"<<iFileSize;

    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&iFileSize,sizeof(qint64));
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::uploadError(QString strError)
{
    QMessageBox::warning(this,"提示",strError);
}


void File::on_downloadFile_PB_clicked()
{
    if(flag){
        QMessageBox::information(this,"提示","已有文件正在下载");
        return;
    }
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem==NULL){
        QMessageBox::information(this,"提示","请选择一个文件");
        return;
    }
    QString downloadFileName = pItem->text();
    QString Path = QFileDialog::getExistingDirectory();
    if(Path.toStdString().size()>32){
        QMessageBox::information(this,"提示","文件夹名长度非法");
        return;
    }
    if(Path.toStdString().size()==0){
        return;
    }
    m_strFilePath = QString("%1/%2").arg(Path).arg(downloadFileName);//要下载的文件路径

    PDU* pdu = mkPDU(32);
    pdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caData,downloadFileName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void File::on_shareFile_PB_clicked()
{
    Index::getInstance().getFriend()->flushFriend();
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem==NULL){
        QMessageBox::information(this,"提示","请选择一个文件");
        return;
    }
//    foreach(FileInfo* pFileInfo,m_pFileList){
//        if(pItem->text()==pFileInfo->caName&&pFileInfo->iType!=1){
//            QMessageBox::information(this,"提示","请选择一个文件");
//            return;
//        }
//    }
    m_strShareFileName = pItem->text();
    m_pShareFile->updateFriend_LW();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }
}
