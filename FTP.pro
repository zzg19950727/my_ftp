#-------------------------------------------------
#
# Project created by QtCreator 2015-09-24T21:01:56
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets winextras

TARGET = FTP
TEMPLATE = app


SOURCES +=\
        mainwindow.cpp \
    main.cpp \
    mainslots.cpp \
    qftp.cpp \
    qftpcommand.cpp \
    qftppi.cpp \
    qftpdtp.cpp \
    qftpprivate.cpp \
    qurlinfo.cpp \
    connect.cpp \
    task.cpp \
    browser.cpp \
    file_action.cpp \
    travel.cpp

HEADERS  += mainwindow.h \
    qurlinfo.h \
    ftp.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
