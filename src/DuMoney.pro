#-------------------------------------------------
#
# Project created by QtCreator 2012-12-13T20:17:37
#
#-------------------------------------------------

QT       += core gui \
        sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DuMoney
TEMPLATE = app


CONFIG += static



SOURCES += main.cpp\
        mainwindow.cpp \
    about.cpp \
    editeurtypes.cpp \
    dufsqlquery.cpp \
    editeurcategories.cpp \
    credit.cpp \
    debit.cpp \
    editeurclients.cpp

HEADERS  += mainwindow.h \
    about.h \
    editeurtypes.h \
    dufsqlquery.h \
    editeurcategories.h \
    credit.h \
    debit.h \
    editeurclients.h

FORMS    += mainwindow.ui \
    about.ui \
    editeurtypes.ui \
    editeurcategories.ui \
    credit.ui \
    debit.ui \
    editeurclients.ui

RESOURCES += \
    ressources.qrc
