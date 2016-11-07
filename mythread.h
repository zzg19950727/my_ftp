#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "ftp.h"
#include "mainwindow.h"

class MyThread
{
public:
    QProgressBar* currentProgress;
    QFtp *ftp;
    QFile *file;
    QString filename_server;
    QString filename_local;
    QQueue<task> queue;
public:
    MyThread(QFtp* ftp, QQueue<task> queue);
    ~MyThread();
    void run();
    void startTask(task T);
private slots:
    void transferProgress(qint64 have, qint64 total);
    //一条命令执行完成时所做的响应
    void ftpCommandFinished(int commandId, bool error);
    //命令开始执行时所做的响应
    void ftpCommandStart(int commandId);
};

#endif // MYTHREAD_H
