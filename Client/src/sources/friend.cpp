#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <qmessagebox.h>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
    flushFriend();
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;
    delete m_pChat;
}

void Friend::flushFriend()
{
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::updateFriendList(QStringList friendList)
{
    ui->listWidget->clear();
    //ui->listWidget->addItems(friendList);
    for(int i = 0;i<friendList.size();i++){
        QListWidgetItem* pItem = new QListWidgetItem;
        pItem->setIcon(QIcon(QPixmap(":/780 (2).png")));
        pItem->setText(friendList[i]);
        ui->listWidget->addItem(pItem);
    }
}

QListWidget *Friend::getFriend_LW()
{
    return ui->listWidget;
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this,"查找用户","用户名:");
    if(strName.toStdString().size()>32){
        QMessageBox::warning(this,"提示","用户名长度非法");
        return;
    }
    if(strName.toStdString().size()==0){
        return;
    }
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_FIND_USER_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
    }
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_flushFriend_PB_clicked()
{
    flushFriend();
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择一个好友");
        return;
    }
    QString strDelName = pItem->text();
    int ret = QMessageBox::question(this,"提示",QString("是否确认删除好友 %1").arg(strDelName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_MSG_TYPE_DEL_FRIEND_REQUEST;
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strDelName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择一个好友");
        return;
    }
    m_pChat->setWindowTitle(pItem->text());
    if(m_pChat->isHidden()){
        m_pChat->show();
    }
    m_pChat->m_strChatName = pItem->text();
}
