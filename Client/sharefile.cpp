#include "client.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    ui->listWidget->clear();
    QListWidget* friendList = Index::getInstance().getFriend()->getFriend_LW();
    for (int i=0; i<friendList->count();i++) {
        QListWidgetItem* pFriendItem = new QListWidgetItem(*friendList->item(i));
        pFriendItem->setIcon(QIcon(QPixmap(":/780 (2).png")));
        ui->listWidget->addItem(pFriendItem);
    }
}

void ShareFile::on_allSelect_PB_clicked()
{
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelect_PB_clicked()
{
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    QString strCurPath = Index::getInstance().getFile()->m_strCurPath;
    QString strFileName = Index::getInstance().getFile()->m_strShareFileName;
    QString strPath = strCurPath+"/"+strFileName;
    QString strCurName = Client::getInstance().m_strLoginName;
    QList<QListWidgetItem*> pItems = ui->listWidget->selectedItems();
    int friend_Num = pItems.size();
    if(friend_Num==0){
        return;
    }

    PDU* pdu = mkPDU(friend_Num*32+strPath.toStdString().size()+1);
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&friend_Num,sizeof(int));
    for(int i = 0;i<friend_Num;i++){
        memcpy((char*)(pdu->caMsg)+i*32, pItems.at(i)->text().toStdString().c_str(), 32);
    }
    memcpy((char*)(pdu->caMsg)+friend_Num*32,strPath.toStdString().c_str(),strPath.size());
    pdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu);
}
