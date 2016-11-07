#include "mainwindow.h"
#include "ui_mainwindow.h"

//点击下载按钮时的响应
void MainWindow::on_download_clicked()
{
    if(ftp->state() == QFtp::Unconnected)
    {
        QMessageBox::warning(this, "错误", "未连接", 0, 0);
        return;
    }
    //若未选择文件
    if(!ui->ServerDir->currentItem())
    {
        QMessageBox::warning(this, "错误", "请选择文件", 0, 0);
        return;
    }

    /*//若选择的是文件夹，不支持文件夹下载
    filename_server = ui->ServerDir->currentItem()->text(0);
    if(isDirectory[filename_server])
    {
        QMessageBox::warning(this, "错误", "请选择单个文件", 0, 0);
        return;
    }
    //获取保存文件的路径
    filename_local.clear();
    //filename_local = QFileDialog::getSaveFileName(this, "保存");
    filename_local = QFileDialog::getExistingDirectory(this, "下载");
    if(filename_local.length() == 0)
        return;
    filename_local += ("/"+filename_server);*/
    QList<QTreeWidgetItem*> item_list = ui->ServerDir->selectedItems();
    QString file_size;
    //获取保存文件的路径
    filename_local.clear();
    filename_local = QFileDialog::getExistingDirectory(this, "下载");
    if(filename_local.length() == 0)
        return;
    for(int i=0; i<item_list.size(); i++)
    {
        filename_server = item_list[i]->text(0);
        if(isDirectory[filename_server])
            continue;
        file_size = item_list[i]->text(1);
        download_file(filename_server, filename_server, file_size);
    }

    ui->startLoad->setEnabled(true);
}

void MainWindow::download_file(QString file_local, QString file_server, QString file_size)
{
    //更改当前状态为下载
    state.clear();
    state = "下载";
    QProgressBar* progress = new QProgressBar();
    progress->setMinimum(0);
    //显示上传或现在的数据信息
    QTreeWidgetItem* item;
    item = new QTreeWidgetItem;
    item->setText(0, state);
    item->setText(1, host);
    item->setText(2, filename_local+"/"+file_local);
    item->setText(3, file_server);
    item->setText(4, file_size);
    item->setText(5, "等待传输");

    //设置图标
    if(state == "上传")
    {
        QPixmap pixmap("::images/up.ico");
        item->setIcon(0, pixmap);
    }
    else
    {
        QPixmap pixmap(":images/down.ico");
        item->setIcon(0, pixmap);
    }
    ui->transfering->addTopLevelItem(item);
    //新建任务
    task download;
    download.isdownload = true;
    download.Spath = currentPath+"/"+file_server;
    download.Dpath = filename_local+"/"+file_local;
    download.progress = progress;
    download.item = item;
    queue.enqueue(download);
}

//点击上传按钮时的响应
void MainWindow::on_upload_clicked()
{
    if(ftp->state() == QFtp::Unconnected)
    {
        QMessageBox::warning(this, "错误", "未连接", 0, 0);
        return;
    }/*
    //若未指定上传目录
    if(!ui->ServerDir->currentItem())
    {
        QMessageBox::warning(this, "错误", "请指定远程文件夹", 0, 0);
        return;
    }

    //获取指定上传目录
    filename_server = ui->ServerDir->currentItem()->text(0);
    //若指定的位置非目录
    if(!isDirectory[filename_server])
    {
        QMessageBox::warning(this, "错误", "请指定远程文件夹", 0, 0);
        return;
    }*/
    filename_server = currentPath;
    //获取预上传的文件路径
    filename_local.clear();
    filename_local = QFileDialog::getOpenFileName(this, "上传");
    if(filename_local.length() == 0)
        return;
    filename_server += filename_local.right(filename_local.length()-filename_local.lastIndexOf("/"));
    //更改当前状态为上传
    state.clear();
    state = "上传";
    QProgressBar* progress = new QProgressBar();
    progress->setMinimum(0);
    //显示上传或现在的数据信息
    QTreeWidgetItem* item;
    item = new QTreeWidgetItem;
    item->setText(0, state);
    item->setText(1, host);
    item->setText(2, filename_local);
    item->setText(3, filename_server);
    item->setText(4, ui->ServerDir->currentItem()->text(1));
    item->setText(5, "等待传输");

    //设置图标
    if(state == "上传")
    {
        QPixmap pixmap(":images/up.ico");
        item->setIcon(0, pixmap);
    }
    else
    {
        QPixmap pixmap(":images/down.ico");
        item->setIcon(0, pixmap);
    }
    ui->transfering->addTopLevelItem(item);
    //新建任务
    task upload;
    upload.isdownload = false;
    upload.Spath = filename_local;
    upload.Dpath = filename_server;
    upload.progress = progress;
    upload.item = item;
    queue.enqueue(upload);

    ui->startLoad->setEnabled(true);
}

//删除文件夹或文件
void MainWindow::on_remove_clicked()
{
    //判断是否选择了文件夹或文件
    if( !ui->ServerDir->currentItem() )
        return;
    name.clear();
    name = ui->ServerDir->currentItem()->text(0);
    //提示用户是否确认删除
    if(name.length() > 6)
    {
        name = name.left(6);
        name += "...";
    }
    if(QMessageBox::warning(this, "警告","是否删除"+name,"删除","取消"))
        return;
    name = ui->ServerDir->currentItem()->text(0);
    //文件夹与文件分开执行
    if(isDirectory[name])
    {
        //获取当前文件夹的绝对路径
        name = currentPath+"/"+name;
        ftp->rmdir(name);
    }
    else
    {
        name = currentPath+"/"+name;
        ftp->remove(name);
    }
    ui->ServerDir->clear();
}

//新建文件夹
void MainWindow::on_mkdir_clicked()
{
    //检查是否连接
    if( ftp->state() == QFtp::Unconnected )
    {
        ui->ConnectState->append("未连接到服务器");
        return;
    }
    //打开输入框获取文件夹名
    name.clear();
    name = QInputDialog::getText(this, "新建文件夹","文件夹名:");
    if( name.length() == 0 )
        return;
    name = currentPath+"/"+name;
    ftp->mkdir(name);
    ui->ServerDir->clear();
}

//更改文件名
void MainWindow::on_rename_clicked()
{
    //判断是否选择了文件夹或文件
    if( !ui->ServerDir->currentItem() )
        return;
    name.clear();
    name = ui->ServerDir->currentItem()->text(0);
    //打开输入框获取新文件名
    QString new_name = QInputDialog::getText(this, "重命名","文件夹名:");
    if( name.length() == 0 )
        return;
    name = currentPath+"/"+name;
    new_name = currentPath+"/"+new_name;
    ftp->rename(name, new_name);
    ui->ServerDir->clear();
}
