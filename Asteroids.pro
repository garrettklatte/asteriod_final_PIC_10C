#-------------------------------------------------
#
# Project created by QtCreator 2015-10-23T09:49:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Asteroids
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    asteroid.cpp

HEADERS  += mainwindow.h \
    asteroid.h

FORMS    += mainwindow.ui \
    outerspace.ui

RESOURCES += \
    res.qrc
