#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

#include <qmessagebox.h>

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateUserList(QStringList userList)
{
    ui->listWidget->clear();
    for(int i = 0;i<userList.size();i++){
        QListWidgetItem* pItem = new QListWidgetItem;
        pItem->setIcon(QIcon(QPixmap(":/780 (2).png")));
        pItem->setText(userList[i]);
        ui->listWidget->addItem(pItem);
    }
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    if(strTarName==strCurName){
        QMessageBox::warning(this,"提示","请选择其他用户");
        return;
    }
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}
