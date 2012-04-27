#-------------------------------------------------
#
# Project created by QtCreator 2012-04-26T21:38:04
#
#-------------------------------------------------

QT       += core gui

TARGET = OntoEditor
TEMPLATE = app

SOURCES += main.cpp\
    RelationVisualizedLine.cpp \
    RelationItem.cpp \
    OntologyWidget.cpp \
    OntologyGraphicsView.cpp \
    NodeItem.cpp \
    OntologyGraphElement.cpp \
    lib_json/json_valueiterator.inl \
    lib_json/json_internalmap.inl \
    lib_json/json_internalarray.inl \
    lib_json/json_writer.cpp \
    lib_json/json_value.cpp \
    lib_json/json_reader.cpp

HEADERS  += RelationVisualizedLine.h \
    RelationItem.h \
    OntologyWidget.h \
    OntologyGraphicsView.h \
    NodeItem.h \
    ItemType.h \
    ItemDataKey.h \
    OntologyGraphElement.h \
    IOntologyWidgetDelegate.h \
    lib_json/json_tool.h \
    lib_json/json_batchallocator.h \
    lib_json/json/writer.h \
    lib_json/json/value.h \
    lib_json/json/reader.h \
    lib_json/json/json.h \
    lib_json/json/forwards.h \
    lib_json/json/features.h \
    lib_json/json/config.h \
    lib_json/json/autolink.h

FORMS += \
    OntologyWidget.ui
