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
  m_ontologyWidget->setDataSource(&m_dataController);
  m_ontologyWidget->setDelegate(&m_dataController);
  ui->contentLayout->addWidget(m_ontologyWidget);

  m_ontologyTreeViewController = new OntologyTreeViewController();
  m_ontologyTreeViewController->setDataSource(&m_dataController);
  m_ontologyTreeViewController->setDelegate(&m_dataController);
  ui->treeViewLayout->addWidget(m_ontologyTreeViewController->treeView());

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 10);

  m_logicalInference = new LogicalInference(&m_dataController, &m_dataController);

  connect(m_ontologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_ontologyWidget, SLOT(dataChangedSlot()));

  connect(m_ontologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_ontologyWidget, SLOT(itemSelectedSlot(long)));

  setupMenu();

  m_zoomInShortcut = new QShortcut(this);
  m_zoomInShortcut->setKey(QKeySequence("Ctrl+="));
  m_zoomInShortcut->setEnabled(true);
  connect(m_zoomInShortcut, SIGNAL(activated()), m_ontologyWidget, SLOT(zoomInSlot()));

  m_zoomOutShortcut = new QShortcut(this);
  m_zoomOutShortcut->setKey(QKeySequence("Ctrl+-"));
  m_zoomOutShortcut->setEnabled(true);
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_ontologyWidget, SLOT(zoomOutSlot()));
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::setupMenu() {

  QMenu *menu = ui->menubar->addMenu(tr("File"));
  QAction *saveAction = menu->addAction(tr("Save..."));
  QAction *loadAction = menu->addAction(tr("Load..."));
  QAction *consultAction = menu->addAction(tr("Consult..."));

  connect(saveAction, SIGNAL(triggered()), SLOT(saveSlot()));
  connect(loadAction, SIGNAL(triggered()), SLOT(loadSlot()));
  connect(consultAction, SIGNAL(triggered()), SLOT(consultSlot()));
}

void MainWindow::saveSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.ojs");
  QFile file(filePath);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  Json::Value jsonState = m_ontologyWidget->serialize();
  jsonState["data_source"] = m_dataController.serialize();

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream.setAutoDetectUnicode(true);
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
      m_dataController = OntologyDataController(jsonState["data_source"]);
      m_ontologyWidget->deserialize(jsonState);
      m_ontologyTreeViewController->updateData();
      if (m_logicalInference != NULL) {
        disconnect(m_logicalInference);
        delete m_logicalInference;
      }
      m_logicalInference = new LogicalInference(&m_dataController, &m_dataController);
      connect(m_ontologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyWidget, SLOT(dataChangedSlot()));
      connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
    }
  }
}

void MainWindow::consultSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*");
  if (QFile::exists(filePath)) {
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(filePath.toStdString().c_str());

    Json::Value jsonState;
    bool ok = reader.parse(fileStream, jsonState);
    if (ok) {
      Json::Value json = m_logicalInference->process(jsonState);
      qDebug() << QString::fromStdString(json.toStyledString());

      QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*");
      QFile file(filePath);

      if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
      }

      QTextStream stream(&file);
      stream.setCodec("UTF-8");
      stream.setAutoDetectUnicode(true);
      stream << QString::fromStdString(json.toStyledString());
    }
  }
}
