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

    OntologyWidget *m_ontologyWidget;
    OntologyTreeViewController *m_ontologyTreeViewController;
    OntologyDataController m_dataController;
    LogicalInference *m_logicalInference;

    QShortcut *m_zoomInShortcut;
    QShortcut *m_zoomOutShortcut;

    void setupMenu();

  private slots:
    void saveSlot();
    void loadSlot();
    void consultSlot();
};

#endif // MAINWINDOW_H
