#include "mainwindow.h"
#include "ui_mainwindow.h"

//开始执行任务
void MainWindow::on_startLoad_clicked()
{
    //检查是否有任务
    if(queue.isEmpty())
    {
        ui->ConnectState->append("当前无准备的任务");
        return;
    }
    //检查是否已达连接上限
    if(ftplist.count() < MAXTASK)
    {
        //发送开始任务信号
        emit StartTrans(queue);
        //清空当前任务队列
        queue.clear();
    }
    else
        ui->ConnectState->append("任务总数已达上限，等待最近任务完成后再继续");
    //startTask(queue.dequeue(),ftp);
    ui->startLoad->setEnabled(false);
}

//将比特数据转换为合适的单位
void MainWindow::GetSize(qint64 s, QString &text)
{
    float size = s;
    char ch[20];
    //1KB以内
    if(size < 1024)
    {
        sprintf(ch, "%.0f", size);
        text = ch;
        text += "B";
        return;
    }
    size /= 1024;
    //1MB以内
    if(size < 1024)
    {
        sprintf(ch, "%.0f", size);
        text = ch;
        text += "KB";
        return;
    }
    size /= 1024;
    //1GB以内
    if(size < 1024)
    {
        sprintf(ch, "%.1f", size);
        text = ch;
        text += "MB";
        return;
    }
    size /= 1024;
    //1TB以内
    if(size < 1024)
    {
        sprintf(ch, "%.2f", size);
        text = ch;
        text += "GB";
        return;
    }
    size /= 1024;
    //1PB以内
    if(size < 1024)
    {
        sprintf(ch, "%.2f", size);
        text = ch;
        text += "TB";
        return;
    }
}

//开始一个任务
void MainWindow::startTask(task T, QFtp* ftp)
{
    //默认下载完整文件
    ftp->base = 0;
    //初始化为正在进行任务
    ftp->isCancled = false;
    ftp->isPause = false;
    //添加暂停按钮
    ftp->pause = new QPushButton("");
    ftp->pause->setMaximumWidth(25);
    QIcon pause("images/Pause.ico");
    ftp->pause->setFlat(true);
    ftp->pause->setIcon(pause);
    ui->transfering->setItemWidget(T.item, 7, ftp->pause);
    connect(ftp->pause, SIGNAL(clicked(bool)), ftp, SLOT(emit_pause()));
    //添加取消按钮
    ftp->cancle = new QPushButton("");
    ftp->cancle->setMaximumWidth(25);
    QIcon cancle("images/close.ico");
    ftp->cancle->setFlat(true);
    ftp->cancle->setIcon(cancle);
    ui->transfering->setItemWidget(T.item, 8, ftp->cancle);
    connect(ftp->cancle, SIGNAL(clicked(bool)), ftp, SLOT(emit_cancle()));
    //保存当前任务对应的菜单栏信息
    ftp->currentItem = T.item;
    //添加当前任务的进度条
    ftp->currentProgress = T.progress;
    T.progress->setMaximum(1);
    T.progress->setValue(0);
    ftp->currentProgress->setMaximumHeight(20);
    ui->transfering->setItemWidget(T.item, 5, T.progress);
    //用于显示当前任务的网速
    ftp->currentLable = new QLabel("");
    ui->transfering->setItemWidget(T.item, 6, ftp->currentLable);
    //保存任务开始的时间
    ftp->start = QDateTime::currentDateTime();
    ftp->have = 0;

    if(T.isdownload)
    {
        //保存当前任务信息
        ftp->isDownload = true;
        ftp->filename_local = T.Dpath;
        ftp->filename_server = T.Spath;
        ftp->file = new QFile(T.Dpath);
        if(!ftp->file->open(QIODevice::WriteOnly))
            return;
        ftp->get(T.Spath, ftp->file);
    }
    else
    {
        //保存当前任务信息
        ftp->isDownload = false;
        ftp->filename_local = T.Spath;
        ftp->filename_server = T.Dpath;
        ui->ConnectState->append(T.Dpath);
        ftp->file = new QFile(T.Spath);
        if(!ftp->file->open(QIODevice::ReadOnly))
            return;
        ftp->put(ftp->file, T.Dpath);
    }
}

//清除正在执行的任务
void MainWindow::DeleteFile()
{
    //删除所有未完成的下载文件
    for(int i=0; i<ftplist.count(); i++)
    {
        if(ftplist.at(i)->isDownload)
            if(ftplist.at(i)->file->exists())
                ftplist.at(i)->file->remove();
    }
}

//数据传输时的响应
void MainWindow::transferProgress(qint64 have, qint64 total)
{
    //获取当前进程
    QFtp *ftp = (QFtp*)this->sender();
    //设置进度条信息
    qint64 recv = have+ftp->base;
    //进度条数据范围检测
    if(total > 2147483647)
    {
        total /= 1024;
        recv /= 1024;
    }
    ftp->currentProgress->setMaximum(total);
    ftp->currentProgress->setValue(recv);
    //计算当前平均网速
    QDateTime now = QDateTime::currentDateTime();
    qint64 msec = ftp->start.msecsTo(now);
    int V = 0;
    //每隔一秒计算一次网速
    if(msec > 1000)
    {
        if(have > ftp->have)
            V = (have-ftp->have)/msec;
        else
            V = 0;
        QString text;
        GetSize(V*1000, text);
        text += "/S";
        ftp->currentLable->setText(text);
        ftp->have = have;
        ftp->start = now;
    }
}

//新建传输数据线程
void MainWindow::Trans(QQueue<task> queue)
{
    if(queue.isEmpty())
        return;
    //新建一个ftp连接
    ftplist.append(new QFtp());
    ftplist.last()->queue = queue;
    //使用当前状态登录登录
    connectToftp(ftplist.last());
    //连接传输信号
    connect(ftplist.last(), SIGNAL(pause_clicked()), this, SLOT(Pause()));
    connect(ftplist.last(), SIGNAL(cancle_clicked()), this, SLOT(Cancle()));
    connect(ftplist.last(), SIGNAL(commandStarted(int)), this, SLOT(commandStart(int)));
    connect(ftplist.last(), SIGNAL(commandFinished(int,bool)), this, SLOT(commandFinished(int,bool)));
    connect(ftplist.last(), SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(transferProgress(qint64,qint64)));
    //开始执行任务
    startTask(ftplist.last()->queue.dequeue(), ftplist.last());
}

//暂停数据传输
void MainWindow::Pause()
{
    //获取当前需要暂停的ftp线程
    QFtp *ftp = (QFtp*)this->sender();
    //若任务正在进行
    if( !ftp->isPause )
    {
        //退出任务
        ftp->abort();
        ftp->file->close();
        if(ftp->isDownload)
        {

            //保存当前已下载的文件大小
            ftp->base = ftp->file->size();
        }
        ftp->isPause = true;
        //设置按钮图标为开始下载
        QIcon start("images/Start.ico");
        ftp->pause->setIcon(start);
        ftp->currentLable->setText("0KB/S");
        ftp->cancle->setEnabled(false);
        return;
    }
    //若当前任务已暂停
    else
    {
        if(ftp->state() == QFtp::Unconnected)
            connectToftp(ftp);
        //设置状态为未暂停
        ftp->isPause = false;
        //设置按钮图标为暂停
        QIcon pause("images/Pause.ico");
        ftp->pause->setIcon(pause);
        //从上次暂停的节点开始传输
        if(ftp->isDownload)
        {
            ftp->file->open(QIODevice::Append);
            ftp->get(ftp->filename_server, ftp->file, ftp->file->size());
        }
        else
        {
            ftp->file->open(QIODevice::ReadOnly);
            ftp->put(ftp->file, ftp->filename_server, true);
        }
        ftp->cancle->setEnabled(true);
    }
}

void MainWindow::Cancle()
{
    //获取当前需要暂停的ftp线程
    QFtp *ftp = (QFtp*)this->sender();
    //退出任务
    ftp->isCancled = true;
    ftp->abort();
    ftplist.removeOne(ftp);
    emit StartTrans(ftp->queue);
}
