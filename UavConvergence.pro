#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T15:20:34
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UavConvergence
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    DataModel.cpp \
    MainPresenter.cpp \
    ConvergenceCalculator.cpp

HEADERS  += MainWindow.h \
    DataModel.h \
    Telemetry.h \
    MainPresenter.h \
    ITelemetryViewer.h \
    ConvergenceCalculator.h

FORMS    += MainWindow.ui

DISTFILES +=

RESOURCES += \
    res/images.qrc
