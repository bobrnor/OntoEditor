#-------------------------------------------------
#
# Project created by QtCreator 2012-04-26T21:38:04
#
#-------------------------------------------------

QT       += core gui

TARGET = OntoEditor
TEMPLATE = app

SOURCES += main.cpp \
    lib_json/json_valueiterator.inl \
    lib_json/json_internalmap.inl \
    lib_json/json_internalarray.inl \
    lib_json/json_writer.cpp \
    lib_json/json_value.cpp \
    lib_json/json_reader.cpp \
    OntologyDataController.cpp \
    MainWindow.cpp \
    ontology/graph/RelationVisualizedLine.cpp \
    ontology/graph/RelationItem.cpp \
    ontology/graph/OntologyWidget.cpp \
    ontology/graph/OntologyGraphicsView.cpp \
    ontology/graph/OntologyGraphElement.cpp \
    ontology/graph/NodeItem.cpp \
    ontology/treeview/OntologyTreeViewController.cpp \
    LogicalInference.cpp \
    JsonToOntoHelper.cpp

HEADERS  += \
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
    ontology/graph/RelationVisualizedLine.h \
    ontology/graph/RelationItem.h \
    ontology/graph/OntologyWidget.h \
    ontology/graph/OntologyGraphicsView.h \
    ontology/graph/OntologyGraphElement.h \
    ontology/graph/NodeItem.h \
    ontology/graph/ItemType.h \
    ontology/graph/ItemDataKey.h \
    ontology/treeview/OntologyTreeViewController.h \
    ontology/IOntologyDelegate.h \
    ontology/IOntologyDataSource.h \
    LogicalInference.h \
    JsonToOntoHelper.h \
    LINodeData.h \
    ontology/NodeData.h \
    ontology/RelationData.h

FORMS += \
    MainWindow.ui \
    ontology/graph/OntologyWidget.ui
