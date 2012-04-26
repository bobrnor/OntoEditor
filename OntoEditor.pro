#-------------------------------------------------
#
# Project created by QtCreator 2012-04-26T10:43:37
#
#-------------------------------------------------

QT       += core gui

TARGET = OntoEditor
TEMPLATE = app


SOURCES += main.cpp\
        OntologyWidget.cpp \
    RelationItem.cpp \
    NodeItem.cpp

HEADERS  += OntologyWidget.h \
    RelationItem.h \
    NodeItem.h \
    ItemDataKey.h \
    ItemType.h

FORMS    += OntologyWidget.ui
