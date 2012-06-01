#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ontology/graph/OntologyWidget.h"
#include "ontology/treeview/OntologyTreeViewController.h"
#include "OntologyDataController.h"
#include "LogicalInference.h"

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

    OntologyWidget *m_domainSpecificOntologyWidget;
    OntologyWidget *m_javaOntologyWidget;
    OntologyWidget *m_objcOntologyWidget;
    OntologyWidget *m_problemsOntologyWidget;

    OntologyTreeViewController *m_ontologyTreeViewController;

    OntologyDataController m_domainSpecificOntologyController;
    OntologyDataController m_javaOntologyController;
    OntologyDataController m_objcOntologyController;
    OntologyDataController m_problemsOntologyController;

    LogicalInference *m_logicalInference;

    QShortcut *m_zoomInShortcut;
    QShortcut *m_zoomOutShortcut;
    QShortcut *m_removeShortcut;

    void setupMenu();

    void setupDomainSpecificOntology();
    void setupJavaOntology();
    void setupObjcOntology();
    void setupProblemsOntology();

    void onDomainSpecificOntologyWidgetShow();
    void onJavaOntologyWidgetShow();
    void onObjcOntologyWidgetShow();
    void onProblemsOntologyWidgetShow();

    void clearConnections();

  private slots:
    void saveSlot();
    void loadSlot();
    void consultSlot();
    void screenshotSlot();
    void currentTabChangedSlot(int index);
};

#endif // MAINWINDOW_H
