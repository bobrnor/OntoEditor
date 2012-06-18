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

    OntologyWidget *m_sourceOntologyWidget;
    OntologyWidget *m_destinationOntologyWidget;
    OntologyWidget *m_javaOntologyWidget;
    OntologyWidget *m_objcOntologyWidget;
    OntologyWidget *m_problemsOntologyWidget;

    OntologyTreeViewController *m_ontologyTreeViewController;

    QMap<QString, IOntologyDataSource *> m_languageDataSources;
    QMap<QString, IOntologyDelegate *> m_languageDelegates;

    OntologyDataController m_sourceOntologyController;
    OntologyDataController m_destinationOntologyController;
    OntologyDataController m_javaOntologyController;
    OntologyDataController m_objcOntologyController;
    OntologyDataController m_problemsOntologyController;

    LogicalInference *m_logicalInference;

    QShortcut *m_zoomInShortcut;
    QShortcut *m_zoomOutShortcut;
    QShortcut *m_removeShortcut;

    void setupMenu();

    void setupSourceOntology();
    void setupDestinationOntology();
    void setupJavaOntology();
    void setupObjcOntology();
    void setupProblemsOntology();

    void onSourceOntologyWidgetShow();
    void onDestinationOntologyWidgetShow();
    void onJavaOntologyWidgetShow();
    void onObjcOntologyWidgetShow();
    void onProblemsOntologyWidgetShow();

    void clearConnections();

    QString findCorrenspondingLanguage(const QString &term) const;

  private slots:
    void openSourceFileSlot();
    void saveGeneratedFileSlot();

    void openWorkspaceSlot();
    void saveWorkspaceSlot();

    void openProjectSlot();
    void saveProjectSlot();

    void screenshotSlot();

    void transformSlot();

    void currentTabChangedSlot(int index);
};

#endif // MAINWINDOW_H
