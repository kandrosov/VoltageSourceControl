#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T11:17:56
#
#-------------------------------------------------

QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += widgets


TARGET = VoltageSourceControl
TEMPLATE = app
QMAKE_CXXFLAGS = -std=c++11

LIBS += -lboost_system -lboost_date_time

SOURCES += main.cpp\
        MainWindow.cpp \
    GpibStream.cc \
    Keithley237.cc \
    Keithley237Internals.cc \
    Keithley6487.cc \
    serialstream.cc \
    ThreadSafeVoltageSource.cc \
    date_time.cc \
    log.cc \
    VoltageSourceFactory.cc \
    BaseConfig.cc \
    Controller.cc \
    GuiController.cpp

HEADERS  += MainWindow.h \
    FakeVoltageSource.h \
    GpibStream.h \
    IVoltageSource.h \
    Keithley237.h \
    Keithley237Internals.h \
    Keithley6487.h \
    serialstream.h \
    ThreadSafeVoltageSource.h \
    units.h \
    date_time.h \
    exception.h \
    log.h \
    VoltageSourceFactory.h \
    ConfigParameters.h \
    BaseConfig.h \
    Controller.h \
    GuiController.h

FORMS    += MainWindow.ui

OTHER_FILES += \
    README.md \
    LICENSE \
    parameters.cfg
