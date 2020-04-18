TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -pthread

SOURCES += \
        main.cpp \
    cell.cpp

HEADERS += \
    printer.h \
    cell.h \
    clustergroup.h
