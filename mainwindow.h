#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAXTASK 4
#include <QAbstractItemModel>
#include <QFileSystemModel>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressBar>
#include <QCloseEvent>
#include <QProcess>
#include <QDateTime>
#include <QTime>
#include <QLabel>
#include <QImage>
#include <QIcon>
#include <QMenu>
#include <QHash>
#include <QFile>
#include <QDir>
#include <QQueue>
#include <QDebug>
#include <stdio.h>
#include <unistd.h>
#include "ftp.h"
namespace Ui {
class MainWindow;
}

struct file_info
{
    QString path;
    QUrlInfo urlInfo;
};

//class QUrlInfo;
//class QFtp;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void contextMenuEvent(QContextMenuEvent *event);
    void init_UI();
    //设置图标
    void SetIcon();
    //设置文件浏览控件的抬头
    void SetHead();
    //连接信号与槽
    void ConnectSignal();
    //连接到ftp服务器
    void connectToftp(QFtp *ftp);
    //断开连接
    void disconnectToftp();
    //开始执行任务
    void startTask(task T, QFtp* ftp);
    //用户关闭程序时的处理
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void GetSize(qint64 size, QString &text);
    void DeleteFile();


private slots:
    //连接状态改变时所做的响应
    void connect_state_changed();
    //一条命令执行完成时所做的响应
    void ftpCommandFinished(int commandId, bool error);

    void commandFinished(int commandId, bool error);
    //命令开始执行时所做的响应
    void ftpCommandStart(int commandId);

    void commandStart(int commandId);
    //显示服务器端的文件列表
    void List(const QUrlInfo &urlInfo);
    //切换文件目录时所做的响应
    void processItem(QTreeWidgetItem *item, int column);
    //数据传输时所做的响应
    void transferProgress(qint64, qint64);
    //开始传输数据
    void Trans(QQueue<task> queue);
    //暂停传输
    void Pause();
    //取消传输
    void Cancle();
    //点击连接按钮时所做的响应
    void on_connect_clicked();
    //点击回退时所做的响应
    void on_cdToParent_clicked();
    //点击刷新按钮时所做的响应
    void on_reflush_clicked();
    //点击下载时所做的响应
    void on_download_clicked();

    void download_file(QString file_local, QString file_server, QString size);
    //点击上传时所做的响应
    void on_upload_clicked();
    //删除文件夹或文件
    void on_remove_clicked();
    //新建文件夹
    void on_mkdir_clicked();
    //重命名
    void on_rename_clicked();
    //开始任务
    void on_startLoad_clicked();

    void write_log(QString str);

private slots:
    void travel(file_info &info);

    void get_list(const QUrlInfo &urlInfo);

    void show_result(file_info &info);

    void search();

    void next_path();

    void on_search_clicked();

    void on_pushButton_stop_clicked();



    void on_pushButton_return_clicked();

    void on_pushButton_reflush_clicked();

signals:
    void StartTrans(QQueue<task> queue);

    void list_finished();


private:
    //界面
    Ui::MainWindow *ui;
    //连接句柄
    QFtp *ftp;
    //当前连接的主机
    QString host;
    //标记服务器端当前目录的文件夹
    QHash<QString, bool> isDirectory;
    //当前访问的远程路径
    QString currentPath;
    //当前下载或上传的远程文件名
    QString filename_server;
    //当前下载或上传的本地文件名
    QString filename_local;
    //文件指针
    QFile *file;
    //数据传输状态，上传/下载
    QString state;
    //开始上传或下载文件的时间
    QDateTime start;
    //临时储存被操作的文件路径
    QString name;
    //任务队列
    QQueue<task> queue;
    //当前任务条
    QProgressBar* currentProgress;
    //当前已连接的服务端
    QList<QFtp*> ftplist;

    QLabel *path;
    QLabel *log;

    QList<file_info> file_list;
    QQueue<file_info> dir_list;
    QList<file_info> result;
    QString path_bak;

    bool stop;

    int index;
};

#endif // MAINWINDOW_H
