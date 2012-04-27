#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <fstream>

#include "lib_json/json/json.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {

  ui->setupUi(this);

  m_ontologyWidget = new OntologyWidget(this);
  m_ontologyWidget->setDelegate(&m_dataController);
  ui->verticalLayout->addWidget(m_ontologyWidget);

  setupMenu();
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::setupMenu() {

  QMenu *menu = ui->menubar->addMenu(tr("File"));
  QAction *saveAction = menu->addAction(tr("Save..."));
  QAction *loadAction = menu->addAction(tr("Load..."));

  connect(saveAction, SIGNAL(triggered()), SLOT(saveSlot()));
  connect(loadAction, SIGNAL(triggered()), SLOT(loadSlot()));
}

void MainWindow::saveSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.ojs");
  QFile file(filePath);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  Json::Value jsonState = m_ontologyWidget->serialize();

  QTextStream stream(&file);
  stream << QString::fromStdString(jsonState.toStyledString());
}

void MainWindow::loadSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*.ojs");
  if (QFile::exists(filePath)) {
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(filePath.toStdString().c_str());

    Json::Value jsonState;
    bool ok = reader.parse(fileStream, jsonState);
    if (ok) {
      m_ontologyWidget->deserialize(jsonState);
    }
  }
}
