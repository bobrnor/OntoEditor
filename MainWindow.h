#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "onto_widget/OntologyWidget.h"
#include "OntologyDataController.h"

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
    OntologyDataController m_dataController;

    void setupMenu();

  private slots:
    void saveSlot();
    void loadSlot();
};

#endif // MAINWINDOW_H
