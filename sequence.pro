QT += core
QT -= gui

CONFIG += c++11

TARGET = sequence
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
    src/cliquenetwork.cpp \
    src/converter.cpp \
    src/utils.cpp \
    src/tournament.cpp \
    src/cliquenetworkmanager.cpp

HEADERS += \
    src/cliquenetwork.h \
    src/converter.h \
    src/utils.h \
    src/tournament.h \
    src/cliquenetworkmanager.h
