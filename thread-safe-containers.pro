TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -pthread

SOURCES += \
        main.cpp

include(src/thread-safe-containers.pri)
include(examples/examples.pri)
