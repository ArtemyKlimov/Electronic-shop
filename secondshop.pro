#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T15:13:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = secondshop
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    add_data.cpp \
    drawgraph.cpp \
    qcustomplot.cpp \
    order.cpp

HEADERS  += mainwindow.h \
    add_data.h \
    drawgraph.h \
    qcustomplot.h \
    order.h

FORMS    += mainwindow.ui \
    add_data.ui \
    drawgraph.ui \
    order.ui
