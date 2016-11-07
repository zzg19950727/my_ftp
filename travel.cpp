#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::travel(file_info &info)
{
    QString path;
    if( info.path.length() == 0 )
        path = currentPath;
    else
    {
        path = info.path+"/"+info.urlInfo.name();
        currentPath = path;
    }

    ftp->cd(path);
    ftp->list();
}

void MainWindow::get_list(const QUrlInfo &urlInfo)
{
    file_info info;
    info.path = currentPath;
    info.urlInfo = urlInfo;

    if(urlInfo.isDir())  
        dir_list.enqueue(info);
    else
    {
        QString str = ui->key_word->text();
        str = str.toLower();
        QString file = urlInfo.name();
        if( file.toLower().contains(str) )
        {
            show_result(info);
        }
    }
}

void MainWindow::show_result(file_info &info)
{
    QUrlInfo &urlInfo = info.urlInfo;
    QString size;
    GetSize(urlInfo.size(), size);
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, info.path+"/"+urlInfo.name());
    item->setText(1, size);
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString("yyyy/M/d"));
    //设置文件图标
    QPixmap *pixmap;

    if(urlInfo.name().contains(".tar")
            || urlInfo.name().contains(".iso")
            || urlInfo.name().contains(".rar")
            || urlInfo.name().contains(".zip")
            || urlInfo.name().contains(".gz"))
        pixmap = new QPixmap("images/rar.ico");
    else if(urlInfo.isDir())
        pixmap = new QPixmap("images/folder.ico");
    else if(urlInfo.name().contains(".exe"))
        pixmap = new QPixmap("images/exe.ico");
    else
        pixmap = new QPixmap("images/file.ico");
    item->setIcon(0, *pixmap);
    //保存夹标记
    isDirectory[urlInfo.name()] = urlInfo.isDir();
    ui->ServerDir->addTopLevelItem(item);
    //默认标记第一选项
    if (!ui->ServerDir->currentItem())
    {
        ui->ServerDir->setCurrentItem(ui->ServerDir->topLevelItem(0));
        ui->ServerDir->setEnabled(true);
    }
}

void MainWindow::search()
{
    disconnect(ftp, SIGNAL(listInfo(QUrlInfo)),\
            this, SLOT(List(QUrlInfo)));

    connect(this, SIGNAL(list_finished()),\
            this, SLOT(next_path()) );

    connect(ftp, SIGNAL(listInfo(QUrlInfo)),\
           this, SLOT(get_list(QUrlInfo)));
    path_bak = currentPath;
    stop = true;

    file_info info;
    info.path = "";
    dir_list.clear();
    travel(info);
}

void MainWindow::next_path()
{
    if( dir_list.empty() )
    {
        write_log("search finished!");
        return;
    }
    file_info info = dir_list.dequeue();
    travel(info);

}

void MainWindow::on_pushButton_stop_clicked()
{
    disconnect(this, SIGNAL(list_finished()),\
            this, SLOT(next_path()) );

    disconnect(ftp, SIGNAL(listInfo(QUrlInfo)),\
               this, SLOT(get_list(QUrlInfo)));

    connect(ftp, SIGNAL(listInfo(QUrlInfo)),\
                this, SLOT(List(QUrlInfo)));

    currentPath = path_bak;

    stop = false;

    write_log("search finished!");
}
