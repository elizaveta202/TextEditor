#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T21:59:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    findreplacedlg.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    findreplacedlg.h
