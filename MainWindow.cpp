#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidget>
#include <fstream>

#include "lib_json/json/json.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {

  ui->setupUi(this);

  setupDomainSpecificOntology();
  setupJavaOntology();
  setupObjcOntology();
  setupProblemsOntology();

  m_ontologyTreeViewController = new OntologyTreeViewController();
  ui->treeViewLayout->addWidget(m_ontologyTreeViewController->treeView());

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 10);

  m_logicalInference = new LogicalInference(&m_domainSpecificOntologyController, &m_domainSpecificOntologyController);

  m_zoomInShortcut = new QShortcut(this);
  m_zoomInShortcut->setKey(QKeySequence("Ctrl+="));
  m_zoomInShortcut->setEnabled(true);

  m_zoomOutShortcut = new QShortcut(this);
  m_zoomOutShortcut->setKey(QKeySequence("Ctrl+-"));
  m_zoomOutShortcut->setEnabled(true);

  m_removeShortcut = new QShortcut(this);
  m_removeShortcut->setKey(QKeySequence::Delete);
  m_removeShortcut->setEnabled(true);

  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(currentTabChangedSlot(int)));

  setupMenu();
  onDomainSpecificOntologyWidgetShow();
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::setupDomainSpecificOntology() {

  m_domainSpecificOntologyWidget = new OntologyWidget(this);
  m_domainSpecificOntologyWidget->setDataSource(&m_domainSpecificOntologyController);
  m_domainSpecificOntologyWidget->setDelegate(&m_domainSpecificOntologyController);

  QWidget *dsTab = ui->tabWidget->widget(0);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  dsTab->setLayout(layout);
  layout->addWidget(m_domainSpecificOntologyWidget);
}

void MainWindow::setupJavaOntology() {

  m_javaOntologyWidget = new OntologyWidget();
  m_javaOntologyWidget->setDataSource(&m_javaOntologyController);
  m_javaOntologyWidget->setDelegate(&m_javaOntologyController);

  QWidget *javaTab = ui->tabWidget->widget(1);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  javaTab->setLayout(layout);
  layout->addWidget(m_javaOntologyWidget);
}

void MainWindow::setupObjcOntology() {

  m_objcOntologyWidget = new OntologyWidget(this);
  m_objcOntologyWidget->setDataSource(&m_objcOntologyController);
  m_objcOntologyWidget->setDelegate(&m_objcOntologyController);

  QWidget *objcTab = ui->tabWidget->widget(2);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  objcTab->setLayout(layout);
  layout->addWidget(m_objcOntologyWidget);
}

void MainWindow::setupProblemsOntology() {

  m_problemsOntologyWidget = new OntologyWidget(this);
  m_problemsOntologyWidget->setDataSource(&m_problemsOntologyController);
  m_problemsOntologyWidget->setDelegate(&m_problemsOntologyController);

  QWidget *problemsTab = ui->tabWidget->widget(3);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  problemsTab->setLayout(layout);
  layout->addWidget(m_problemsOntologyWidget);
}

void MainWindow::onDomainSpecificOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_domainSpecificOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_domainSpecificOntologyController);

  connect(m_domainSpecificOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_domainSpecificOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_domainSpecificOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_domainSpecificOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_domainSpecificOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_domainSpecificOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_domainSpecificOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_domainSpecificOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
}

void MainWindow::onJavaOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_javaOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_javaOntologyController);

  connect(m_javaOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_javaOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_javaOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_javaOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_javaOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
}

void MainWindow::onObjcOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_objcOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_objcOntologyController);

  connect(m_objcOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_objcOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_objcOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_objcOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_objcOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
}

void MainWindow::onProblemsOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_problemsOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_problemsOntologyController);

  connect(m_problemsOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_problemsOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_problemsOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_problemsOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_problemsOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
}

void MainWindow::clearConnections() {

  disconnect(m_ontologyTreeViewController);
  disconnect(m_domainSpecificOntologyWidget);
}

void MainWindow::setupMenu() {

  QMenu *menu = ui->menubar->addMenu(tr("File"));
  QAction *saveAction = menu->addAction(tr("Save..."));
  QAction *loadAction = menu->addAction(tr("Load..."));
  QAction *consultAction = menu->addAction(tr("Consult..."));
  QAction *screenshotAction = menu->addAction(tr("Screenshot..."));

  connect(saveAction, SIGNAL(triggered()), SLOT(saveSlot()));
  connect(loadAction, SIGNAL(triggered()), SLOT(loadSlot()));
  connect(consultAction, SIGNAL(triggered()), SLOT(consultSlot()));
  connect(screenshotAction, SIGNAL(triggered()), SLOT(screenshotSlot()));
}

void MainWindow::saveSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.ojs");
  QFile file(filePath);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  Json::Value jsonState;
  jsonState["ds_positions"] = m_domainSpecificOntologyWidget->serialize();
  jsonState["java_positions"] = m_javaOntologyWidget->serialize();
  jsonState["objc_positions"] = m_objcOntologyWidget->serialize();
  jsonState["problems_positions"] = m_problemsOntologyWidget->serialize();

  jsonState["ds_data_source"] = m_domainSpecificOntologyController.serialize();
  jsonState["java_data_source"] = m_javaOntologyController.serialize();
  jsonState["objc_data_source"] = m_objcOntologyController.serialize();
  jsonState["problems_data_source"] = m_problemsOntologyController.serialize();

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
      m_domainSpecificOntologyController = OntologyDataController(jsonState["ds_data_source"]);
      m_javaOntologyController = OntologyDataController(jsonState["java_data_source"]);
      m_objcOntologyController = OntologyDataController(jsonState["objc_data_source"]);
      m_problemsOntologyController = OntologyDataController(jsonState["problems_data_source"]);

      m_domainSpecificOntologyWidget->deserialize(jsonState["ds_positions"]);
      m_javaOntologyWidget->deserialize(jsonState["java_positions"]);
      m_objcOntologyWidget->deserialize(jsonState["objc_positions"]);
      m_problemsOntologyWidget->deserialize(jsonState["problems_positions"]);

      m_ontologyTreeViewController->updateData();
      if (m_logicalInference != NULL) {
        disconnect(m_logicalInference);
        delete m_logicalInference;
      }
      m_logicalInference = new LogicalInference(&m_domainSpecificOntologyController, &m_domainSpecificOntologyController);
      connect(m_domainSpecificOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_domainSpecificOntologyWidget, SLOT(dataChangedSlot()));
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

    Q_ASSERT(ok);

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

void MainWindow::screenshotSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.png");
  QImage screenshot = m_domainSpecificOntologyWidget->makeScreenshot();
  screenshot.save(filePath);
}

void MainWindow::currentTabChangedSlot(int index) {

  switch (index) {
    case 0:
      onDomainSpecificOntologyWidgetShow();
      break;

    case 1:
      onJavaOntologyWidgetShow();
      break;

    case 2:
      onObjcOntologyWidgetShow();
      break;

    case 3:
      onProblemsOntologyWidgetShow();
      break;
  }
}
