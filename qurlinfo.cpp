#include "qurlinfo.h"
#include "qurl.h"
#include "qdir.h"
#include <limits.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

QUrlInfo::QUrlInfo()
{
    d = 0;
}

QUrlInfo::QUrlInfo(const QUrlInfo &ui)
{
    if (ui.d) {
        d = new QUrlInfoPrivate;
        *d = *ui.d;
    } else {
        d = 0;
    }
}

QUrlInfo::QUrlInfo(const QString &name, int permissions, const QString &owner,
                    const QString &group, qint64 size, const QDateTime &lastModified,
                    const QDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
                    bool isWritable, bool isReadable, bool isExecutable)
{
    d = new QUrlInfoPrivate;
    d->name = name.toUtf8().data();
    d->permissions = permissions;
    d->owner = owner;
    d->group = group;
    d->size = size;
    d->lastModified = lastModified;
    d->lastRead = lastRead;
    d->isDir = isDir;
    d->isFile = isFile;
    d->isSymLink = isSymLink;
    d->isWritable = isWritable;
    d->isReadable = isReadable;
    d->isExecutable = isExecutable;
}

QUrlInfo::QUrlInfo(const QUrl &url, int permissions, const QString &owner,
                    const QString &group, qint64 size, const QDateTime &lastModified,
                    const QDateTime &lastRead, bool isDir, bool isFile, bool isSymLink,
                    bool isWritable, bool isReadable, bool isExecutable)
{
    d = new QUrlInfoPrivate;
    d->name = QFileInfo(url.path()).fileName();
    d->permissions = permissions;
    d->owner = owner;
    d->group = group;
    d->size = size;
    d->lastModified = lastModified;
    d->lastRead = lastRead;
    d->isDir = isDir;
    d->isFile = isFile;
    d->isSymLink = isSymLink;
    d->isWritable = isWritable;
    d->isReadable = isReadable;
    d->isExecutable = isExecutable;
}

void QUrlInfo::setName(const QString &name)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->name = name;
}

void QUrlInfo::setDir(bool b)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->isDir = b;
}

void QUrlInfo::setFile(bool b)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->isFile = b;
}

void QUrlInfo::setSymLink(bool b)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->isSymLink = b;
}

void QUrlInfo::setWritable(bool b)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->isWritable = b;
}

void QUrlInfo::setReadable(bool b)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->isReadable = b;
}

void QUrlInfo::setOwner(const QString &s)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->owner = s;
}

void QUrlInfo::setGroup(const QString &s)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->group = s;
}

void QUrlInfo::setSize(qint64 size)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->size = size;
}

void QUrlInfo::setPermissions(int p)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->permissions = p;
}

void QUrlInfo::setLastModified(const QDateTime &dt)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->lastModified = dt;
}

void QUrlInfo::setLastRead(const QDateTime &dt)
{
    if (!d)
        d = new QUrlInfoPrivate;
    d->lastRead = dt;
}

QUrlInfo::~QUrlInfo()
{
    delete d;
}

QUrlInfo &QUrlInfo::operator=(const QUrlInfo &ui)
{
    if (ui.d) {
        if (!d)
            d= new QUrlInfoPrivate;
        *d = *ui.d;
    } else {
        delete d;
        d = 0;
    }
    return *this;
}

QString QUrlInfo::name() const
{
    if (!d)
        return QString();
    return d->name;
}

int QUrlInfo::permissions() const
{
    if (!d)
        return 0;
    return d->permissions;
}

QString QUrlInfo::owner() const
{
    if (!d)
        return QString();
    return d->owner;
}

QString QUrlInfo::group() const
{
    if (!d)
        return QString();
    return d->group;
}

qint64 QUrlInfo::size() const
{
    if (!d)
        return 0;
    return d->size;
}

QDateTime QUrlInfo::lastModified() const
{
    if (!d)
        return QDateTime();
    return d->lastModified;
}

QDateTime QUrlInfo::lastRead() const
{
    if (!d)
        return QDateTime();
    return d->lastRead;
}

bool QUrlInfo::isDir() const
{
    if (!d)
        return false;
    return d->isDir;
}

bool QUrlInfo::isFile() const
{
    if (!d)
        return false;
    return d->isFile;
}

bool QUrlInfo::isSymLink() const
{
    if (!d)
        return false;
    return d->isSymLink;
}

bool QUrlInfo::isWritable() const
{
    if (!d)
        return false;
    return d->isWritable;
}


bool QUrlInfo::isReadable() const
{
    if (!d)
        return false;
    return d->isReadable;
}

bool QUrlInfo::isExecutable() const
{
    if (!d)
        return false;
    return d->isExecutable;
}

bool QUrlInfo::greaterThan(const QUrlInfo &i1, const QUrlInfo &i2,
                            int sortBy)
{
    switch (sortBy) {
    case QDir::Name:
        return i1.name() > i2.name();
    case QDir::Time:
        return i1.lastModified() > i2.lastModified();
    case QDir::Size:
        return i1.size() > i2.size();
    default:
        return false;
    }
}

bool QUrlInfo::lessThan(const QUrlInfo &i1, const QUrlInfo &i2,
                         int sortBy)
{
    return !greaterThan(i1, i2, sortBy);
}

bool QUrlInfo::equal(const QUrlInfo &i1, const QUrlInfo &i2,
                      int sortBy)
{
    switch (sortBy) {
    case QDir::Name:
        return i1.name() == i2.name();
    case QDir::Time:
        return i1.lastModified() == i2.lastModified();
    case QDir::Size:
        return i1.size() == i2.size();
    default:
        return false;
    }
}

bool QUrlInfo::operator==(const QUrlInfo &other) const
{
    if (!d)
        return other.d == 0;
    if (!other.d)
        return false;

    return (d->name == other.d->name &&
            d->permissions == other.d->permissions &&
            d->owner == other.d->owner &&
            d->group == other.d->group &&
            d->size == other.d->size &&
            d->lastModified == other.d->lastModified &&
            d->lastRead == other.d->lastRead &&
            d->isDir == other.d->isDir &&
            d->isFile == other.d->isFile &&
            d->isSymLink == other.d->isSymLink &&
            d->isWritable == other.d->isWritable &&
            d->isReadable == other.d->isReadable &&
            d->isExecutable == other.d->isExecutable);
}

bool QUrlInfo::isValid() const
{
    return d != 0;
}

QT_END_NAMESPACE
