QT += core
QT -= gui

CONFIG += c++11

TARGET = sequence
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    cliquenetwork.cpp \
    converter.cpp \
    utils.cpp \
    tournament.cpp \
    cliquenetworkmanager.cpp

HEADERS += \
    cliquenetwork.h \
    converter.h \
    utils.h \
    tournament.h \
    cliquenetworkmanager.h
