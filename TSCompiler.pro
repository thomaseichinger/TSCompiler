#-------------------------------------------------
#
# Project created by QtCreator 2011-06-02T16:39:55
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = TSCompiler
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    CoCo/Action.cpp \
    CoCo/ArrayList.cpp \
    CoCo/BitArray.cpp \
    CoCo/CharClass.cpp \
    CoCo/CharSet.cpp \
    CoCo/Coco.cpp \
    CoCo/Comment.cpp \
    CoCo/DFA.cpp \
    CoCo/Generator.cpp \
    CoCo/HashTable.cpp \
    CoCo/Melted.cpp \
    CoCo/Node.cpp \
    CoCo/Parser.cpp \
    CoCo/ParserGen.cpp \
    CoCo/Position.cpp \
    CoCo/Scanner.cpp \
    CoCo/SortedList.cpp \
    CoCo/State.cpp \
    CoCo/StringBuilder.cpp \
    CoCo/Symbol.cpp \
    CoCo/Tab.cpp \
    CoCo/Target.cpp

OTHER_FILES += \
    CoCo/build.sh \
    CoCo/coc.sh \
    CoCo/Parser.frame \
    CoCo/Scanner.frame

HEADERS += \
    CoCo/Action.h \
    CoCo/ArrayList.h \
    CoCo/BitArray.h \
    CoCo/CharClass.h \
    CoCo/CharSet.h \
    CoCo/Comment.h \
    CoCo/DFA.h \
    CoCo/Generator.h \
    CoCo/Graph.h \
    CoCo/HashTable.h \
    CoCo/Node.h \
    CoCo/Parser.h \
    CoCo/ParserGen.h \
    CoCo/Position.h \
    CoCo/Scanner.h \
    CoCo/Sets.h \
    CoCo/SortedList.h \
    CoCo/State.h \
    CoCo/StringBuilder.h \
    CoCo/Symbol.h \
    CoCo/Tab.h \
    CoCo/Target.h
