#include "mainwindow.h"
#include "ui_mainwindow.h"

//显示服务端的目录
void MainWindow::List(const QUrlInfo &urlInfo)
{
    QString size;
    GetSize(urlInfo.size(), size);
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
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
        pixmap = new QPixmap(":images/rar.ico");
    else if(urlInfo.isDir())
        pixmap = new QPixmap(":images/folder.ico");
    else if(urlInfo.name().contains(".exe"))
        pixmap = new QPixmap(":images/exe.ico");
    else
        pixmap = new QPixmap(":images/file.ico");
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
    ui->lineEdit_path->setText(currentPath);
}

//切换目录时的响应
void MainWindow::processItem(QTreeWidgetItem *item, int /*column*/ )
{
    if(!item)
        return;
    //获取要切换的目录名
    QString name = item->text(0);
    //若未连接到服务端
    if(ftp->state() == QFtp::Unconnected)
    {
        //ui->ConnectState->append("请刷新目录");
        connectToftp(ftp);
        //return;
    }
    //若是目录则切换，否则不响应
    if (isDirectory.value(name))
    {
        //清空显示区
        ui->ServerDir->clear();
        //清空标记
        isDirectory.clear();
        //跳转目录
        currentPath += '/';
        currentPath += name;
        ftp->cd(currentPath);
        //显示目录
        ftp->list();
    }
}

//点击回退按钮时的响应
void MainWindow::on_cdToParent_clicked()
{
    //若未连接则重新连接
    if(ftp->state() == QFtp::Unconnected)
        connectToftp(ftp);
    ui->ServerDir->clear();
    isDirectory.clear();
    //获取上一级目录路径
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    //跳转到上一级目录并显示
    if (currentPath.isEmpty())
    {
        ftp->cd("/");
        currentPath.clear();
        currentPath = "/";
    }
    else
    {
        ftp->cd(currentPath);
    }
    ftp->list();
}

//按刷新按钮时的响应
void MainWindow::on_reflush_clicked()
{
    //若未连接则重新连接
    if(ftp->state() == QFtp::Unconnected)
        connectToftp(ftp);
    ui->ServerDir->clear();
    //若当前路径为空则默认为根目录
    if (currentPath.isEmpty())
    {
        ftp->cd("/");
        currentPath.clear();
        currentPath = "/";
    }
    //重新显示目录
    else
    {
        ftp->cd(currentPath);
    }
    ftp->list();
}
