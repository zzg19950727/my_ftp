#ifndef QFTP_H
#define QFTP_H
//#define QFTPDTP_DEBUG

#include <QtCore/qstring.h>
#include <QtNetwork/QtNetwork>
#include <QtCore/qobject.h>
#include <qcoreapplication.h>
#include <qtcpsocket.h>
#include <qstringlist.h>
#include <QProgressBar>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QDateTime>
#include <QLabel>
#include <QDebug>
#include <qregexp.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qtcpserver.h>
#include <qlocale.h>
#include "qurlinfo.h"

struct task
{
    bool isdownload;
    QString Spath;
    QString Dpath;
    QProgressBar* progress;
    QTreeWidgetItem* item;
};

QT_BEGIN_HEADER
class QFtpPI;
class QFtpPrivate;

class QFtp : public QObject
{
    Q_OBJECT

public:
    explicit QFtp(QObject *parent = 0);
    virtual ~QFtp();

    enum State {
        Unconnected,
        HostLookup,
        Connecting,
        Connected,
        LoggedIn,
        Closing
    };
    enum Error {
        NoError,
        UnknownError,
        HostNotFound,
        ConnectionRefused,
        NotConnected
    };
    enum Command {
        None,
        SetTransferMode,
        SetProxy,
        ConnectToHost,
        Login,
        Close,
        List,
        Cd,
        Get,
        Put,
        Remove,
        Mkdir,
        Rmdir,
        Rename,
        RawCommand
    };
    enum TransferMode {
        Active,
        Passive
    };
    enum TransferType {
        Binary,
        Ascii
    };

    int setProxy(const QString &host, quint16 port);
    int connectToHost(const QString &host, quint16 port=21);
    int login(const QString &user = QString(), const QString &password = QString());
    int close();
    int setTransferMode(TransferMode mode);
    int list(const QString &dir = QString());
    int cd(const QString &dir);
    int get(const QString &file, QIODevice *dev=0, qulonglong size=0, TransferType type = Binary);
    int put(const QByteArray &data, const QString &file, TransferType type = Binary);
    int put(QIODevice *dev, const QString &file, bool isBreak = false, TransferType type = Binary);
    int remove(const QString &file);
    int mkdir(const QString &dir);
    int rmdir(const QString &dir);
    int rename(const QString &oldname, const QString &newname);

    int rawCommand(const QString &command);

    qint64 bytesAvailable() const;
    qint64 read(char *data, qint64 maxlen);
    QByteArray readAll();

    int currentId() const;
    QIODevice* currentDevice() const;
    Command currentCommand() const;
    bool hasPendingCommands() const;
    void clearPendingCommands();

    State state() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void abort();
    void emit_pause();
    void emit_cancle();

Q_SIGNALS:
    void stateChanged(int);
    void listInfo(const QUrlInfo&);
    void readyRead();
    void dataTransferProgress(qint64, qint64);
    void rawCommandReply(int, const QString&);

    void commandStarted(int);
    void commandFinished(int, bool);
    void done(bool);

    void pause_clicked();
    void cancle_clicked();

private:
    Q_DISABLE_COPY(QFtp)
    QScopedPointer<QFtpPrivate> d;
public:
    QString host;
     QString filename_server;
    QString filename_local;
    QFile *file;
    QQueue<task> queue;
    QDateTime start;
    qint64 have;
    QLabel* currentLable;
    QProgressBar* currentProgress;
    QTreeWidgetItem* currentItem;
    QPushButton* pause;
    QPushButton* cancle;
    bool isPause;
    bool isCancled;
    bool isDownload;
    qint64 base;

    Q_PRIVATE_SLOT(d, void _q_startNextCommand())
    Q_PRIVATE_SLOT(d, void _q_piFinished(const QString&))
    Q_PRIVATE_SLOT(d, void _q_piError(int, const QString&))
    Q_PRIVATE_SLOT(d, void _q_piConnectState(int))
    Q_PRIVATE_SLOT(d, void _q_piFtpReply(int, const QString&))
};

class QFtpDTP : public QObject
{
    Q_OBJECT

public:
    enum ConnectState {
        CsHostFound,
        CsConnected,
        CsClosed,
        CsHostNotFound,
        CsConnectionRefused
    };

    QFtpDTP(QFtpPI *p, QObject *parent = 0);

    void setData(QByteArray *);
    void setDevice(QIODevice *);
    void writeData();
    void setBytesTotal(qint64 bytes);

    bool hasError() const;
    QString errorMessage() const;
    void clearError();

    void connectToHost(const QString & host, quint16 port);
    int setupListener(const QHostAddress &address);
    void waitForConnection();

    QTcpSocket::SocketState state() const;
    qint64 bytesAvailable() const;
    qint64 read(char *data, qint64 maxlen);
    QByteArray readAll();

    void abortConnection();

    static bool parseDir(const QByteArray &buffer, const QString &userName, QUrlInfo *info);

signals:
    void listInfo(const QUrlInfo&);
    void readyRead();
    void dataTransferProgress(qint64, qint64);

    void connectState(int);

private slots:
    void socketConnected();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError);
    void socketConnectionClosed();
    void socketBytesWritten(qint64);
    void setupSocket();

    void dataReadyRead();

private:
    void clearData();

    QTcpSocket *socket;
    QTcpServer listener;

    QFtpPI *pi;
    QString err;
    qint64 bytesDone;
    qint64 bytesTotal;
    bool callWriteData;

    union {
        QByteArray *ba;
        QIODevice *dev;
    } data;
    bool is_ba;

    QByteArray bytesFromSocket;
};

class QFtpPI : public QObject
{
    Q_OBJECT

public:
    QFtpPI(QObject *parent = 0);

    void connectToHost(const QString &host, quint16 port);

    bool sendCommands(const QStringList &cmds);
    bool sendCommand(const QString &cmd)
        { return sendCommands(QStringList(cmd)); }

    void clearPendingCommands();
    void abort();

    QString currentCommand() const
        { return currentCmd; }

    bool rawCommand;
    bool transferConnectionExtended;

    QFtpDTP dtp;

signals:
    void connectState(int);
    void finished(const QString&);
    void error(int, const QString&);
    void rawFtpReply(int, const QString&);

private slots:
    void hostFound();
    void connected();
    void connectionClosed();
    void delayedCloseFinished();
    void readyRead();
    void error(QAbstractSocket::SocketError);

    void dtpConnectState(int);

private:
    enum State {
        Begin,
        Idle,
        Waiting,
        Success,
        Failure
    };

    enum AbortState {
        None,
        AbortStarted,
        WaitForAbortToFinish
    };

    bool processReply();
    bool startNextCmd();

    QTcpSocket commandSocket;
    QString replyText;
    char replyCode[3];
    State state;
    AbortState abortState;
    QStringList pendingCommands;
    QString currentCmd;

    bool waitForDtpToConnect;
    bool waitForDtpToClose;

    QByteArray bytesFromSocket;

    friend class QFtpDTP;
};

class QFtpCommand
{
public:
    QFtpCommand(QFtp::Command cmd, QStringList raw, const QByteArray &ba);
    QFtpCommand(QFtp::Command cmd, QStringList raw, QIODevice *dev = 0);
    ~QFtpCommand();

    int id;
    QFtp::Command command;
    QStringList rawCmds;

    union {
        QByteArray *ba;
        QIODevice *dev;
    } data;
    bool is_ba;

    static QBasicAtomicInt idCounter;
};

class QFtpPrivate
{
    Q_DECLARE_PUBLIC(QFtp)
public:

    inline QFtpPrivate(QFtp *owner) : close_waitForStateChange(false), state(QFtp::Unconnected),
        transferMode(QFtp::Passive), error(QFtp::NoError), q_ptr(owner)
    { }

    ~QFtpPrivate() { while (!pending.isEmpty()) delete pending.takeFirst(); }

    // private slots
    void _q_startNextCommand();
    void _q_piFinished(const QString&);
    void _q_piError(int, const QString&);
    void _q_piConnectState(int);
    void _q_piFtpReply(int, const QString&);

    int addCommand(QFtpCommand *cmd);

    QFtpPI pi;
    QList<QFtpCommand *> pending;
    bool close_waitForStateChange;
    QFtp::State state;
    QFtp::TransferMode transferMode;
    QFtp::Error error;
    QString errorString;

    QString host;
    quint16 port;
    QString proxyHost;
    quint16 proxyPort;
    QFtp *q_ptr;
};
QT_END_HEADER

#endif // QFTP_H
