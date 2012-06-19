#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ontology/graph/OntologyWidget.h"
#include "ontology/treeview/OntologyTreeViewController.h"
#include "OntologyDataController.h"
#include "LogicalInference.h"
#include "Project.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
  private:
    Ui::MainWindow *ui;

    Project m_currentProject;
    QString m_currentFileName;

    OntologyWidget *m_sourceOntologyWidget;
    OntologyWidget *m_destinationOntologyWidget;
    OntologyWidget *m_javaOntologyWidget;
    OntologyWidget *m_objcOntologyWidget;
    OntologyWidget *m_problemsOntologyWidget;

    OntologyTreeViewController *m_ontologyTreeViewController;

    LogicalInference *m_logicalInference;

    QShortcut *m_zoomInShortcut;
    QShortcut *m_zoomOutShortcut;
    QShortcut *m_removeShortcut;

    void setupMenu();

    void setupSourceOntologyWidget();
    void setupDestinationOntologyWidget();
    void setupJavaOntologyWidget();
    void setupObjcOntologyWidget();
    void setupProblemsOntologyWidget();

    void onSourceOntologyWidgetShow();
    void onDestinationOntologyWidgetShow();
    void onJavaOntologyWidgetShow();
    void onObjcOntologyWidgetShow();
    void onProblemsOntologyWidgetShow();

    void clearConnections();

    void updateOntologyTreeData();

  private slots:
    void importSourceFileSlot();
    void exportGeneratedFileSlot();

    void openWorkspaceSlot();
    void saveWorkspaceSlot();

    void openProjectSlot();
    void saveProjectSlot();

    void screenshotSlot();

    void transformSlot();

    void currentTabChangedSlot(int index);
};

#endif // MAINWINDOW_H
