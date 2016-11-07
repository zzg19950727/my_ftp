#include "mythread.h"

MyThread::MyThread(QFtp *ftp, QQueue<task> queue)
{
    this->ftp = ftp;
    this->queue = queue;
    //connect(ftp, SIGNAL(commandFinished(int,bool)),this, SLOT(ftpCommandFinished(int,bool)));
    //connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStart(int)));
    //connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(transferProgress(qint64,qint64)));
}

void MyThread::run()
{
    startTask(queue.dequeue());
}

void MyThread::startTask(task T)
{
    if(T.isdownload)
    {
        filename_local = T.Dpath;
        filename_server = T.Spath;
        currentProgress = T.progress;
       // ui->transfering->setItemWidget(T.item, 4, T.progress);
        file = new QFile(T.Dpath);
        if(!file->open(QIODevice::WriteOnly))
            return;
        ftp->get(T.Spath, file);
    }
    else
    {
        filename_local = T.Spath;
        filename_server = T.Dpath;
        currentProgress = T.progress;
        //ui->transfering->setItemWidget(T.item, 4, T.progress);
        file = new QFile(T.Spath);
        if(!file->open(QIODevice::ReadOnly))
            return;
        ftp->put(file, T.Dpath);
    }
}

void MyThread::transferProgress(qint64 have, qint64 total)
{
    currentProgress->setMaximum(total);
    currentProgress->setValue(have);
}

void MyThread::ftpCommandFinished(int commandId, bool error)
{
    QDateTime now = QDateTime::currentDateTime();
    //若完成下载指令
    if (ftp->currentCommand() == QFtp::Get)
    {
        //若出现错误
        if (error)
        {
            //关闭文件并删除破损文件
            file->close();
            if(file->exists())
                file->remove();
            //添加下载日志
            currentProgress->setValue(currentProgress->maximum());
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "下载");
            item->setText(1, filename_local);
            item->setText(2, filename_server);
            item->setText(3, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(4, "传输失败");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            //ui->transferfailed->addTopLevelItem(item);
            //ui->ConnectState->append("下载"+filename_server+"失败\n"+ftp->errorString());
            qDebug()<<"下载"+filename_server+"失败\n"+ftp->errorString();
        }
        //若成功下载
        else
        {
            //关闭文件流
            file->close();
            //添加下载日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "下载");
            item->setText(1, filename_local);
            item->setText(2, filename_server);
            item->setText(3, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(4, "传输成功");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            //ui->transfersuccess->addTopLevelItem(item);
            //ui->ConnectState->append("下载"+filename_server+"成功");
            qDebug()<<"下载"+filename_server+"成功";
        }
        if(!queue.isEmpty())
            startTask(queue.dequeue());
        return;
    }
    //若完成上传指令
    else if (ftp->currentCommand() == QFtp::Put)
    {
        //若出错
        if (error)
        {
            //关闭文件流
            file->close();
            //添加上传日志
            currentProgress->setValue(currentProgress->maximum());
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "上传");
            item->setText(1, filename_local);
            item->setText(2, filename_server);
            item->setText(3, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(4, "传输失败");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            //ui->transferfailed->addTopLevelItem(item);
            //ui->ConnectState->append("上传"+filename_local+"失败\n"+ftp->errorString());
        }
        //若成功上传
        else
        {
            //关闭文件流
            file->close();
            //添加上传日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "上传");
            item->setText(1, filename_local);
            item->setText(2, filename_server);
            item->setText(3, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(4, "传输成功");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            //ui->transfersuccess->addTopLevelItem(item);
            //ui->ConnectState->append("上传"+filename_local+"成功");
        }
        if(!queue.isEmpty())
            startTask(queue.dequeue());
        return;
    }
}

void MyThread::ftpCommandStart(int commandId)
{
    /*if (ftp->currentCommand() == QFtp::Get)
    {
        ui->ConnectState->append("正在下载"+filename_server);
    }
    else if (ftp->currentCommand() == QFtp::Put)
    {
        ui->ConnectState->append("正在上传"+filename_local);
    }*/
}

