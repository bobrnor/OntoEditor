#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "core/OntologyDataController.h"
#include "ui/ontology/graph/OntologyWidget.h"
#include "ui/ontology/treeview/OntologyTreeViewController.h"
#include "ui/project_tree/ProjectTreeViewController.h"
#include "ui/project_tree/Project.h"

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

    QList<OntologyWidget *> m_openOntologyWidgets;

    QTreeView *m_logTreeView;

    ProjectTreeViewController *m_projectTreeViewController;
    OntologyTreeViewController *m_ontologyTreeViewController;

    long m_currentSnapshotIndex;

    QShortcut *m_zoomInShortcut;
    QShortcut *m_zoomOutShortcut;
    QShortcut *m_removeShortcut;

    QMap<QString, QString> m_convertersMapping;
    QMap<QString, QString> m_transformatorsMapping;

    void setupMenu();
    void setupConverters();
    void setupTransformators();

    OntologyWidget *createNewOntologyWidget(ProjectFile *file);

    void clearConnections();
    void updateOntologyTreeData();

  private slots:
    void importSourceFileSlot();

    void openOntologyFileSlot();
    void saveOntologyFileSlot();

    void openProjectSlot();
    void saveProjectSlot();

    void exportFileSlot();
    void screenshotSlot();

    void transformSlot();

    void moveToStartSlot();
    void moveForwardSlot();
    void moveBackwardSlot();
    void moveToEndSlot();

    void currentTabChangedSlot(int index);
    void currentFileChangedSlot(const QString &fileName);
    void categorySelectedSlot(const QString &fileName, const QString &categoryName);

  signals:
    void itemsSelectedSignal(const QSet<long> ids);
};

#endif // MAINWINDOW_H
