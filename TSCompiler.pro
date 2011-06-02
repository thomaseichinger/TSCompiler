#-------------------------------------------------
#
# Project created by QtCreator 2011-06-02T16:39:55
#
#-------------------------------------------------

QT       -= core

QT       -= gui

COCOTARGET.target = Parser.cpp Parser.h Scanner.cpp Scanner.h
COCOTARGET.commands = ./CoCo/Coco ./src/TSCompiler.atg
COCOTARGET.depends = TSCompiler.atg

TARGET = TSCompiler
TARGET.depends = COCOTARGET
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += src/main.cpp \
    src/Parser.cpp \
    src/Scanner.cpp \
    src/DFA.cpp \
    src/Tab.cpp

OTHER_FILES += \
    src/Parser.frame \
    src/Scanner.frame \
    src/TSCompiler.atg

HEADERS += src/Parser.h \
    src/Scanner.h \
    src/DFA.h \
    src/Tab.h

