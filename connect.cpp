#include "mainwindow.h"
#include "ui_mainwindow.h"

//点击连接按钮时的响应
void MainWindow::on_connect_clicked()
{
    //若当前未连接，则连接
    if(ftp->state() == QFtp::Unconnected)
    {
        connectToftp(ftp);
        currentPath.clear();
        currentPath = "/";  //设置默认访问路径
        ftp->cd(currentPath);
        ftp->list();    //显示目录文件
        ui->ServerDir->clear();
    }
    //若已连接则断开连接
    else
    {
        disconnectToftp();
    }
}

//连接到ftp客户端
void MainWindow::connectToftp(QFtp *ftp)
{
    //如果当前已连接，则直接返回
    if( ftp->state() == QFtp::Connected ||
            ftp->state() == QFtp::LoggedIn )
        ftp->close();
    //获取主机名
    host = ui->host->text();
    if(!host.length())
        return;
    //获取端口号
    QString port_str = ui->port->text();
    qint16 port = port_str.toInt();
    if( port == 0 )
        port = 21;  //默认端口号为21
    if(ui->transferMode->currentIndex() == 1)
        ftp->setTransferMode(QFtp::Active);
    else
        ftp->setTransferMode(QFtp::Passive);
    //连接到服务器
    ftp->connectToHost(host, port);
    //获取用户名
    QString user = ui->user->text();
    //获取密码
    QString passwd = ui->passwd->text();
    //若未填写用户名则匿名登录
    if(!user.length())
        ftp->login();
    else
        ftp->login(user, passwd);
    //ui->ServerDir->clear();
}

//断开连接
void MainWindow::disconnectToftp()
{
    //若已断开则直接返回
    if( ftp->state()==QFtp::Unconnected ||
            ftp->state()==QFtp::Closing )
        return;
    else
        //关闭连接
        ftp->close();
}

//状态改变时的响应
void MainWindow::connect_state_changed()
{
    //获取当前的连接状态
    int state = ftp->state();
    QString str;
    switch(state)
    {
    case QFtp::Unconnected:{
        str = "已断开连接";
        QIcon conn("images/connect.ico");
        //ui->connect->setIcon(conn);
        ui->connect->setText("连接");
        break;}
    case QFtp::HostLookup:
        str = "正在解析主机";
        break;
    case QFtp::Connecting:
        str = "连接中...";
        break;
    case QFtp::Connected:
        str = "已连接到"+host;
        break;
    case QFtp::LoggedIn:{
        str = "已登录到"+host;
        //设置连接按钮为断开连接态
        QIcon conn("images/disconnect.ico");
        //ui->connect->setIcon(conn);
        ui->connect->setText("断开");
        break;}
    case QFtp::Closing:
        str = "正在断开连接...";
        break;
    default:
        str = "";
    }
    //显示状态信息
    ui->ConnectState->append(str);
    log->setText(str);
}
