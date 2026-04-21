#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QFile>
#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strUserPath;
    QString m_strCurPath;
    void flushFile();
    void updataFileList(QList<FileInfo *> pFileList);
    QList<FileInfo*> m_pFileList;
    QString m_strMvName;
    QString m_strMvPath;
    QString m_strUploadFilePath;
    QString m_strFilePath;//保存路径
    qint64 m_iDownloadFileSize;//文件总大小
    qint64 m_iReceiveSize;//已经下载的大小
    QFile m_fDownloadFile;//下载到
    bool flag = false;//是否已有文件正在下载
    void uploadFile();
    void downloadFile();
    void downloadFileData(PDU* pdu);
    ShareFile* m_pShareFile;
    QString m_strShareFileName;


private slots:
    void on_mkDir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_delFile_PB_clicked();

    void on_delDir_PB_clicked();

    void on_renameFile_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_mvFile_PB_clicked();

    void on_uploadFile_PB_clicked();
    void on_downloadFile_PB_clicked();

    void on_shareFile_PB_clicked();

public slots:
    void uploadError(QString strError);

private:
    Ui::File *ui;
};

#endif // FILE_H
