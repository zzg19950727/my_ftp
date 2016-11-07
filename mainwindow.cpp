#include "mainwindow.h"
#include "ui_mainwindow.h"

//初始化窗口
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ftp = new QFtp();
    stop = false;
    index = 0;
    host = QString();
    currentProgress = NULL;
    init_UI();
}

//删除窗口
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_UI()
{
    QPalette palette;
    palette.setBrush(QPalette::Background,QBrush(QPixmap(":images/background.jpg")));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    log = new QLabel();
    log->setMinimumWidth(100);
    ui->statusBar->addWidget(log);

    path = new QLabel();
    ui->statusBar->addWidget(path);

    ui->ConnectState->setMaximumHeight(100);
    ui->port->setMaximumWidth(50);
    ui->ConnectState->setMinimumHeight(300);

    ui->label->setMinimumWidth(54);
    ui->label_2->setMinimumWidth(54);
    ui->label_3->setMinimumWidth(54);
    ui->label_4->setMinimumWidth(54);

    ui->host->setMinimumWidth(140);
    ui->port->setMinimumWidth(140);
    ui->user->setMinimumWidth(140);
    ui->passwd->setMinimumWidth(140);

    ui->widget_login->setMaximumHeight(75);
    //ui->ConnectState->setMinimumHeight(396);
    ui->key_word->setMaximumWidth(120);

    /*ui->host->setStyleSheet("background:transparent;");
    ui->port->setStyleSheet("background:transparent;");
    ui->user->setStyleSheet("background:transparent;");
    ui->passwd->setStyleSheet("background:transparent;");
    ui->transferMode->setStyleSheet("background:transparent;");
    ui->connect->setStyleSheet("background:transparent;");
    ui->ConnectState->setStyleSheet("background:transparent;");
    ui->tabWidget_2->setStyleSheet("background:transparent;");
    ui->ServerDir->setStyleSheet("background:transparent;");
    ui->transfering->setStyleSheet("background:transparent;");
    ui->transferfailed->setStyleSheet("background:transparent;");
    ui->transfersuccess->setStyleSheet("background:transparent;");*/

    SetIcon();
    SetHead();
    ConnectSignal();

    connect(ui->ServerDir, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(processItem(QTreeWidgetItem*,int)));
}

//设置图标
void MainWindow::SetIcon()
{
    QIcon ftp(":images/ftp.ico");
    QIcon Return(":images/back.ico");
    QIcon refresh(":images/refresh_1.ico");
    QIcon start(":images/Start.ico");
    QIcon search(":images/search.ico");

    this->setWindowIcon(ftp);
    ui->connect->setMaximumWidth(50);
    ui->startLoad->setFlat(true);
    ui->startLoad->setIcon(start);
    ui->startLoad->setEnabled(false);

    ui->pushButton_return->setFlat(true);
    ui->pushButton_return->setIcon(Return);
    ui->pushButton_return->setIconSize(QSize(30,30));

    ui->pushButton_reflush->setFlat(true);
    ui->pushButton_reflush->setIcon(refresh);
    ui->pushButton_reflush->setIconSize(QSize(30,30));

    ui->search->setFlat(true);
    ui->search->setIcon(search);
    ui->search->setIconSize(QSize(30,30));


}

//设置文件浏览控件的抬头
void MainWindow::SetHead()
{
    ui->ServerDir->setHeaderLabels(QStringList()\
               << tr("Name") << tr("Size")\
               << tr("Owner") << tr("Group")\
               << tr("Time"));
    ui->ServerDir->header()->setStretchLastSection(false);

    ui->ServerDir->setColumnWidth(0, 150);
    ui->ServerDir->setColumnWidth(1, 50);
    ui->ServerDir->setColumnWidth(2, 50);
    ui->ServerDir->setColumnWidth(3, 50);

    ui->transfering->setHeaderLabels(QStringList()\
               <<tr("方向")<<tr("服务器")\
               <<tr("本地文件")<<tr("远程文件")\
               <<tr("大小")<<tr("状态")<<tr("速度")\
               <<tr("")<<tr(""));
    ui->transfering->header()->setStretchLastSection(false);
    ui->transfering->setColumnWidth(0, 80);
    ui->transfering->setColumnWidth(5, 100);
    ui->transfering->setColumnWidth(7, 25);
    ui->transfering->setColumnWidth(8, 25);
    ui->transfering->setLineWidth(5);

    ui->transferfailed->setHeaderLabels(QStringList()\
               << tr("方向") << tr("服务器")\
               << tr("本地文件") << tr("远程文件")\
               << tr("时间") << tr("状态"));
    ui->transferfailed->header()->setStretchLastSection(false);

    ui->transfersuccess->setHeaderLabels(QStringList()\
                                         << tr("方向") << tr("服务器")\
                                         << tr("本地文件") << tr("远程文件")\
                                         << tr("时间") << tr("状态"));
    ui->transfersuccess->header()->setStretchLastSection(false);
}

//连接信号
void MainWindow::ConnectSignal()
{
    connect(ftp, SIGNAL(stateChanged(int)),\
            this, SLOT(connect_state_changed()));
    connect(ftp, SIGNAL(commandFinished(int,bool)),\
            this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)),\
            this, SLOT(List(QUrlInfo)));
    connect(ftp, SIGNAL(commandStarted(int)),\
            this, SLOT(ftpCommandStart(int)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),\
            this, SLOT(transferProgress(qint64,qint64)));
    connect(this, SIGNAL(StartTrans(QQueue<task>)),\
            this, SLOT(Trans(QQueue<task>)));
    connect(ui->lineEdit_path, SIGNAL(editingFinished()),\
             this, SLOT(on_pushButton_reflush_clicked()));

}

//关闭程序时的响应
void MainWindow::closeEvent(QCloseEvent *event)
{
    //若当前有任务正在进行
    if(ftplist.count() != 0)
    {
        if(QMessageBox::warning(this, "警告", "是否终止所有任务？","确定", "取消") == 0)
        {
            //清除任务
            DeleteFile();
            //关闭程序
            event->accept();
        }

        else
            //忽略信号
            event->ignore();
    }
    //若无任务进行
    else
        //关闭程序
        event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt)
    {
        index = ui->tabWidget_2->currentIndex();
        index ++;
        index = index % ui->tabWidget_2->count();
        ui->tabWidget_2->setCurrentIndex(index);
    }
    else if(event->key() == Qt::Key_Escape)
    {
        on_cdToParent_clicked();
    }
}

void MainWindow::write_log(QString str)
{
    ui->ConnectState->append(str);
    log->setText(str);
}

void MainWindow::on_search_clicked()
{
    QString str = ui->key_word->text();
    if( str.length() == 0 )
        return;
    ui->ServerDir->clear();
    if( stop )
    {
        QIcon search(":images/search.ico");
        ui->search->setIcon(search);
        on_pushButton_stop_clicked();
    }
    else
    {
        QIcon stop(":images/stop.ico");
        ui->search->setIcon(stop);
        search();
    }

}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if( ui->tabWidget_2->currentIndex() != 1 )
        return;
    if(ui->ServerDir->currentItem() == NULL)
        return;

    QIcon icon_upload(":images/upload.ico");
    QIcon icon_download(":images/download.ico");
    QIcon icon_remove(":images/remove.ico");
    QIcon icon_mkdir(":images/mkdir.ico");
    QIcon icon_rename(":images/rename.ico");

    QMenu *menu = new QMenu(this);
    QAction* download = new QAction(icon_download, "下载", this);
    QAction* upload = new QAction(icon_upload, "上传", this);
    QAction* rename = new QAction(icon_rename, "重命名", this);
    QAction* remove = new QAction(icon_remove, "删除", this);
    QAction* mkdir = new QAction(icon_mkdir, "新建文件夹", this);

    connect(download, SIGNAL(triggered(bool)),
            this, SLOT(on_download_clicked()) );

    connect(upload, SIGNAL(triggered(bool)),
            this, SLOT(on_upload_clicked()) );

    connect(rename, SIGNAL(triggered(bool)),
            this, SLOT(on_rename_clicked()) );

    connect(remove, SIGNAL(triggered(bool)),
            this, SLOT(on_remove_clicked()) );

    connect(mkdir, SIGNAL(triggered(bool)),
            this, SLOT(on_mkdir_clicked()) );

    menu->addAction(download);
    menu->addAction(upload);
    menu->addAction(rename);
    menu->addAction(remove);
    menu->addAction(mkdir);

    menu->move( this->cursor().pos() );
    menu->show();
}

void MainWindow::on_pushButton_return_clicked()
{
    on_cdToParent_clicked();
}

void MainWindow::on_pushButton_reflush_clicked()
{
    on_reflush_clicked();
    currentPath = ui->lineEdit_path->text();
}
