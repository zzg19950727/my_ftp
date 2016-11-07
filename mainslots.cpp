#include "mainwindow.h"
#include "ui_mainwindow.h"

//命令完成时的响应
void MainWindow::ftpCommandFinished(int /*commandId*/, bool error)
{
    QDateTime now = QDateTime::currentDateTime();
    //若完成连接指令
    if (ftp->currentCommand() == QFtp::ConnectToHost)
    {
        //若出现错误
        if (error)
        {
            QString text = "无法连接到"+host+"\n";
            text += ftp->errorString();
            write_log(text);
            //ftp->close();
            return;
        }
    }
    //若完成登录指令
    else if (ftp->currentCommand() == QFtp::Login)
    {
        //若出现错误
        if( error )
        {
            QString text = "登录失败\n";
            text += ftp->errorString();
            write_log(text);
            ftp->close();
            return;
        }
    }
    //若完成下载指令
    else if (ftp->currentCommand() == QFtp::Get)
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
            item->setText(1, host);
            item->setText(2, filename_local);
            item->setText(3, filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            ui->transferfailed->addTopLevelItem(item);
            QString text = "下载"+filename_server+"失败\n"+ftp->errorString();
            write_log(text);
        }
        //若成功下载
        else
        {
            //关闭文件流
            file->close();
            //添加下载日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "下载");
            item->setText(1, host);
            item->setText(2, filename_local);
            item->setText(3, filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            ui->transfersuccess->addTopLevelItem(item);
            write_log("下载"+filename_server+"成功");
        }
        if(!queue.isEmpty())
            //startTask(queue.dequeue());
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
            item->setText(1, host);
            item->setText(2, filename_local);
            item->setText(3, filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            ui->transferfailed->addTopLevelItem(item);
            write_log("上传"+filename_local+"失败\n"+ftp->errorString());
        }
        //若成功上传
        else
        {
            //关闭文件流
            file->close();
            //添加上传日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "上传");
            item->setText(1, host);
            item->setText(2, filename_local);
            item->setText(3, filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            ui->transfersuccess->addTopLevelItem(item);
            write_log("上传"+filename_local+"成功");
        }
        if(!queue.isEmpty())
            //startTask(queue.dequeue());
        return;
    }
    //若完成获取文件目录指令
    else if (ftp->currentCommand() == QFtp::List)
    {
        if(error)
        {
            QString text = "读取"+currentPath+"目录失败\n"+ftp->errorString();
            write_log(text);     //日志记录
            emit list_finished();
        }
        else
        {
            QString text = "读取"+currentPath+"目录成功";
            write_log(text);     //日志记录
            emit list_finished();
        }
    }
    //若完成删除文件指令
    else if(ftp->currentCommand() == QFtp::Remove)
    {
        if(error)
        {
            QString text = "删除"+name+"失败\n";
            text += ftp->errorString();
            write_log(text);     //日志记录
        }
        else
        {
            QString text = "删除"+name+"成功";
            write_log(text);     //日志记录
        }
    }
    //若完成删除文件夹指令
    else if(ftp->currentCommand() == QFtp::Rmdir)
    {
        if(error)
        {
            QString text = "删除"+name+"失败\n";
            text += ftp->errorString();
            write_log(text);     //日志记录
        }
        else
        {
            QString text = "删除"+name+"成功";
            write_log(text);     //日志记录
            ftp->list(currentPath);
        }
    }
    //若完成创建文件夹指令
    else if(ftp->currentCommand() == QFtp::Mkdir)
    {
        if(error)
        {
            QString text = "创建"+name+"失败\n";
            text += ftp->errorString();
            write_log(text);     //日志记录
        }
        else
        {
            QString text = "创建"+name+"成功";
            write_log(text);     //日志记录
            ftp->list(currentPath);
        }
    }
    //若完成更改文件名指令
    else if(ftp->currentCommand() == QFtp::Rename)
    {
        if(error)
        {
            QString text = "重命名"+name+"失败\n";
            text += ftp->errorString();
            write_log(text);     //日志记录
        }
        else
        {
            QString text = "重命名"+name+"成功";
            write_log(text);     //日志记录
            ftp->list(currentPath);
        }
    }
}

void MainWindow::commandFinished(int /*commandId*/, bool error)
{
    QFtp *ftp = (QFtp*)this->sender();
    QDateTime now = QDateTime::currentDateTime();
    //若完成下载指令
    if (ftp->currentCommand() == QFtp::Get)
    {
        //若出现错误
        if (error || ftp->isCancled)
        {
            //if(ftp->errorString() == "Failure writing network stream.")
            if(ftp->isPause)
            {
                write_log("已暂停下载"+ftp->filename_server);
                return;
            }
            //关闭文件并删除破损文件
            ftp->file->close();
            if(ftp->file->exists())
                ftp->file->remove();
            //添加下载日志
            //ftp->currentProgress->setValue(ftp->currentProgress->maximum());
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "下载");
            item->setText(1, ftp->host);
            item->setText(2, ftp->filename_local);
            item->setText(3, ftp->filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            ui->transferfailed->addTopLevelItem(item);
            write_log("下载"+ftp->filename_server+"失败\n"+ftp->errorString());
            ftp->currentLable->setText("失败");
            //关闭暂停按钮
            disconnect(ftp->pause, SIGNAL(clicked(bool)), ftp, SLOT(emit_pause()));
            ftp->pause->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 7);
            //关闭取消按钮
            disconnect(ftp->cancle, SIGNAL(clicked(bool)), ftp, SLOT(emit_cancle()));
            ftp->cancle->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 8);
            delete ftp->currentItem;
            ftp->currentItem = NULL;
        }
        //若成功下载
        else
        {
            if(ftp->isPause)
            {
                write_log("已暂停下载"+ftp->filename_server);
                return;
            }
            //关闭文件流
            ftp->file->close();
            //添加下载日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "下载");
            item->setText(1, ftp->host);
            item->setText(2, ftp->filename_local);
            item->setText(3, ftp->filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输成功");
            QPixmap pixmap("images/down.ico");
            item->setIcon(0, pixmap);
            ui->transfersuccess->addTopLevelItem(item);
            write_log("下载"+ftp->filename_server+"成功");
            ftp->currentLable->setText("完成");
            //关闭暂停按钮
            disconnect(ftp->pause, SIGNAL(clicked(bool)), ftp, SLOT(emit_pause()));
            ftp->pause->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 7);
            //关闭取消按钮
            disconnect(ftp->cancle, SIGNAL(clicked(bool)), ftp, SLOT(emit_cancle()));
            ftp->cancle->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 8);
            delete ftp->currentItem;
            ftp->currentItem = NULL;
        }
        if(ftp->isCancled)
        {
            ftp->close();
            return;
        }
        if(!ftp->queue.isEmpty())
            startTask(ftp->queue.dequeue(), ftp);
        else
        {
            ftplist.removeAt(ftplist.indexOf(ftp));
            ftp->abort();
            ftp->close();
            //delete ftp;
        }
        return;
    }
    //若完成上传指令
    else if (ftp->currentCommand() == QFtp::Put)
    {
        //若出错
        if (error || ftp->isCancled)
        {
            if(ftp->isPause)
            {
                write_log("已暂停上传"+ftp->filename_server);
                return;
            }

            //关闭文件流
            ftp->file->close();
            //添加上传日志
            //ftp->currentProgress->setValue(ftp->currentProgress->maximum());
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "上传");
            item->setText(1, ftp->host);
            item->setText(2, ftp->filename_local);
            item->setText(3, ftp->filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输失败");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            ui->transferfailed->addTopLevelItem(item);
            write_log("上传"+ftp->filename_local+"失败\n"+ftp->errorString());
            ftp->currentLable->setText("失败");
            //关闭暂停按钮
            disconnect(ftp->pause, SIGNAL(clicked(bool)), ftp, SLOT(emit_pause()));
            ftp->pause->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 7);
            //关闭取消按钮
            disconnect(ftp->cancle, SIGNAL(clicked(bool)), ftp, SLOT(emit_cancle()));
            ftp->cancle->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 8);
            delete ftp->currentItem;
            ftp->currentItem = NULL;
        }
        //若成功上传
        else
        {
            if(ftp->isPause)
            {
                write_log("已暂停上传"+ftp->filename_server);
                return;
            }
            //关闭文件流
            ftp->file->close();
            //添加上传日志
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "上传");
            item->setText(1, ftp->host);
            item->setText(2, ftp->filename_local);
            item->setText(3, ftp->filename_server);
            item->setText(4, now.toString("yyyy/M/d hh:mm:ss"));
            item->setText(5, "传输成功");
            QPixmap pixmap("images/up.ico");
            item->setIcon(0, pixmap);
            ui->transfersuccess->addTopLevelItem(item);
            write_log("上传"+ftp->filename_local+"成功");
            ftp->currentLable->setText("完成");
            //关闭暂停按钮
            disconnect(ftp->pause, SIGNAL(clicked(bool)), ftp, SLOT(emit_pause()));
            ftp->pause->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 7);
            //关闭取消按钮
            disconnect(ftp->cancle, SIGNAL(clicked(bool)), ftp, SLOT(emit_cancle()));
            ftp->cancle->close();
            ui->transfering->removeItemWidget(ftp->currentItem, 8);
            delete ftp->currentItem;
            ftp->currentItem = NULL;
        }

        if(!ftp->queue.isEmpty())
            startTask(ftp->queue.dequeue(), ftp);
        else
        {
            ftplist.removeAt(ftplist.indexOf(ftp));
            ftp->abort();
            ftp->close();
            //delete ftp;
        }
        return;
    }
}

//命令开始执行时的响应
void MainWindow::ftpCommandStart(int /*commandId*/)
{
    //显示当前执行的指令
    if (ftp->currentCommand() == QFtp::Login)
    {
        write_log("正在登录");
    }
    else if (ftp->currentCommand() == QFtp::Get)
    {
        write_log("正在下载"+filename_server);
        start = QDateTime::currentDateTime();
    }
    else if (ftp->currentCommand() == QFtp::Put)
    {
        write_log("正在上传"+filename_local);
        start = QDateTime::currentDateTime();
    }
    else if (ftp->currentCommand() == QFtp::List)
    {
        QString text = "正在读取"+currentPath+"目录";
        write_log(text);
    }
    else if(ftp->currentCommand() == QFtp::Remove)
    {
        QString text = "正在删除"+name;
        write_log(text);
    }
    else if(ftp->currentCommand() == QFtp::Rmdir)
    {
        QString text = "正在删除"+name;
        write_log(text);
    }
}

void MainWindow::commandStart(int /*commandId*/)
{
    QFtp *ftp = (QFtp*)this->sender();
    if (ftp->currentCommand() == QFtp::Get)
    {
        write_log("正在下载"+ftp->filename_server);
        start = QDateTime::currentDateTime();
    }
    else if (ftp->currentCommand() == QFtp::Put)
    {
        write_log("正在上传"+ftp->filename_local);
        start = QDateTime::currentDateTime();
    }
}
