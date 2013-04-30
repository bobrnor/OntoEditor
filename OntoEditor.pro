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
    core/TransformationHelper.cpp \
    core/OntoToJsonHelper.cpp \
    core/OntologyDataController.cpp \
    core/JsonToOntoHelper.cpp \
    ui/MainWindow.cpp \
    ui/ontology/graph/RelationVisualizedLine.cpp \
    ui/ontology/graph/RelationItem.cpp \
    ui/ontology/graph/OntologyWidget.cpp \
    ui/ontology/graph/OntologyGraphicsView.cpp \
    ui/ontology/graph/OntologyGraphElement.cpp \
    ui/ontology/graph/NodeItem.cpp \
    ui/ontology/treeview/OntologyTreeViewController.cpp \
    ui/project_tree/ProjectTreeWidget.cpp \
    ui/project_tree/ProjectTreeViewController.cpp \
    ui/project_tree/ProjectFileCategory.cpp \
    ui/project_tree/ProjectFile.cpp \
    ui/project_tree/Project.cpp \
    core/Snapshot.cpp

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
    core/TransformationHelper.h \
    core/RelationData.h \
    core/OntoToJsonHelper.h \
    core/OntologyDataController.h \
    core/NodeData.h \
    core/JsonToOntoHelper.h \
    ui/MainWindow.h \
    ui/ontology/graph/RelationVisualizedLine.h \
    ui/ontology/graph/RelationItem.h \
    ui/ontology/graph/OntologyWidget.h \
    ui/ontology/graph/OntologyGraphicsView.h \
    ui/ontology/graph/OntologyGraphElement.h \
    ui/ontology/graph/NodeItem.h \
    ui/ontology/graph/ItemType.h \
    ui/ontology/graph/ItemDataKey.h \
    ui/ontology/treeview/OntologyTreeViewController.h \
    ui/project_tree/ProjectTreeWidgetDelegate.h \
    ui/project_tree/ProjectTreeWidget.h \
    ui/project_tree/ProjectTreeViewController.h \
    ui/project_tree/ProjectFileCategory.h \
    ui/project_tree/ProjectFile.h \
    ui/project_tree/Project.h \
    core/Snapshot.h

FORMS += \
    MainWindow.ui \
    OntologyWidget.ui
