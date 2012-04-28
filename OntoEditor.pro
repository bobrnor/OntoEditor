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
    lib_json/json_reader.cpp \
    OntologyDataController.cpp \
    MainWindow.cpp \
    onto_widget/RelationVisualizedLine.cpp \
    onto_widget/RelationItem.cpp \
    onto_widget/OntologyWidget.cpp \
    onto_widget/OntologyGraphicsView.cpp \
    onto_widget/OntologyGraphElement.cpp \
    onto_widget/OntologyDataController.cpp \
    onto_widget/NodeItem.cpp

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
    lib_json/json/autolink.h \
    IOntologyDataSource.h \
    OntologyDataController.h \
    MainWindow.h \
    onto_widget/RelationVisualizedLine.h \
    onto_widget/RelationItem.h \
    onto_widget/OntologyWidget.h \
    onto_widget/OntologyGraphicsView.h \
    onto_widget/OntologyGraphElement.h \
    onto_widget/OntologyDataController.h \
    onto_widget/NodeItem.h \
    onto_widget/ItemType.h \
    onto_widget/ItemDataKey.h \
    onto_widget/IOntologyWidgetDelegate.h \
    onto_widget/IOntologyDataSource.h

FORMS += \
    OntologyWidget.ui \
    MainWindow.ui \
    onto_widget/OntologyWidget.ui
