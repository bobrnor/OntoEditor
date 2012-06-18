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

  setupSourceOntology();
  setupDestinationOntology();
  setupJavaOntology();
  setupObjcOntology();
  setupProblemsOntology();

  m_ontologyTreeViewController = new OntologyTreeViewController();
  ui->treeViewLayout->addWidget(m_ontologyTreeViewController->treeView());

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 10);

  m_logicalInference = new LogicalInference();
  m_logicalInference->setSourceOntology(&m_sourceOntologyController, &m_sourceOntologyController);
  m_logicalInference->setDestinationOntology(&m_destinationOntologyController, &m_destinationOntologyController);
  m_logicalInference->setProblemsOntology(&m_problemsOntologyController, &m_problemsOntologyController);
  m_logicalInference->addLanguage("java", &m_javaOntologyController, &m_javaOntologyController);
  m_logicalInference->addLanguage("objective-c", &m_objcOntologyController, &m_objcOntologyController);

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
  onSourceOntologyWidgetShow();
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::setupSourceOntology() {

  m_sourceOntologyWidget = new OntologyWidget(this);
  m_sourceOntologyWidget->setDataSource(&m_sourceOntologyController);
  m_sourceOntologyWidget->setDelegate(&m_sourceOntologyController);

  QWidget *dsTab = ui->tabWidget->widget(0);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  dsTab->setLayout(layout);
  layout->addWidget(m_sourceOntologyWidget);
}

void MainWindow::setupDestinationOntology() {

  m_destinationOntologyWidget = new OntologyWidget(this);
  m_destinationOntologyWidget->setDataSource(&m_destinationOntologyController);
  m_destinationOntologyWidget->setDelegate(&m_destinationOntologyController);

  QWidget *dsTab = ui->tabWidget->widget(1);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  dsTab->setLayout(layout);
  layout->addWidget(m_destinationOntologyWidget);
}

void MainWindow::setupJavaOntology() {

  m_javaOntologyWidget = new OntologyWidget();
  m_javaOntologyWidget->setDataSource(&m_javaOntologyController);
  m_javaOntologyWidget->setDelegate(&m_javaOntologyController);

  QWidget *javaTab = ui->tabWidget->widget(2);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  javaTab->setLayout(layout);
  layout->addWidget(m_javaOntologyWidget);
}

void MainWindow::setupObjcOntology() {

  m_objcOntologyWidget = new OntologyWidget(this);
  m_objcOntologyWidget->setDataSource(&m_objcOntologyController);
  m_objcOntologyWidget->setDelegate(&m_objcOntologyController);

  QWidget *objcTab = ui->tabWidget->widget(3);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  objcTab->setLayout(layout);
  layout->addWidget(m_objcOntologyWidget);
}

void MainWindow::setupProblemsOntology() {

  m_problemsOntologyWidget = new OntologyWidget(this);
  m_problemsOntologyWidget->setDataSource(&m_problemsOntologyController);
  m_problemsOntologyWidget->setDelegate(&m_problemsOntologyController);

  QWidget *problemsTab = ui->tabWidget->widget(4);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  problemsTab->setLayout(layout);
  layout->addWidget(m_problemsOntologyWidget);
}

void MainWindow::onSourceOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_sourceOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_sourceOntologyController);

  connect(m_sourceOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_sourceOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_sourceOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_sourceOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_sourceOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_sourceOntologyWidget->updateData();
}

void MainWindow::onDestinationOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataSource(&m_destinationOntologyController);
  m_ontologyTreeViewController->setDelegate(&m_destinationOntologyController);

  connect(m_destinationOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_destinationOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_destinationOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));

  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_destinationOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_destinationOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_destinationOntologyWidget->updateData();
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
  m_javaOntologyWidget->updateData();
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
  m_objcOntologyWidget->updateData();
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
  m_problemsOntologyWidget->updateData();
}

void MainWindow::clearConnections() {

  disconnect(m_ontologyTreeViewController);
  disconnect(m_sourceOntologyWidget);
  disconnect(m_destinationOntologyWidget);
  disconnect(m_javaOntologyWidget);
  disconnect(m_objcOntologyWidget);
  disconnect(m_problemsOntologyWidget);
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
  jsonState["source"] = m_sourceOntologyController.serialize();
  jsonState["destination"] = m_destinationOntologyController.serialize();
  jsonState["java"] = m_javaOntologyController.serialize();
  jsonState["objc"] = m_objcOntologyController.serialize();
  jsonState["problems"] = m_problemsOntologyController.serialize();

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
      m_sourceOntologyController = OntologyDataController(jsonState["source"]);
      m_destinationOntologyController = OntologyDataController(jsonState["destination"]);
      m_javaOntologyController = OntologyDataController(jsonState["java"]);
      m_objcOntologyController = OntologyDataController(jsonState["objc"]);
      m_problemsOntologyController = OntologyDataController(jsonState["problems"]);

      m_sourceOntologyWidget->dataChangedSlot();
      m_destinationOntologyWidget->dataChangedSlot();
      m_javaOntologyWidget->dataChangedSlot();
      m_objcOntologyWidget->dataChangedSlot();
      m_problemsOntologyWidget->dataChangedSlot();

      m_ontologyTreeViewController->updateData();
      if (m_logicalInference != NULL) {
        disconnect(m_logicalInference);
        delete m_logicalInference;
      }
      m_logicalInference = new LogicalInference();
      m_logicalInference->setSourceOntology(&m_sourceOntologyController, &m_sourceOntologyController);
      m_logicalInference->setDestinationOntology(&m_destinationOntologyController, &m_destinationOntologyController);
      m_logicalInference->setProblemsOntology(&m_problemsOntologyController, &m_problemsOntologyController);
      m_logicalInference->addLanguage("java", &m_javaOntologyController, &m_javaOntologyController);
      m_logicalInference->addLanguage("objective-c", &m_objcOntologyController, &m_objcOntologyController);
      connect(m_sourceOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_destinationOntologyWidget, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_logicalInference, SLOT(dataChangedSlot()));
      connect(m_logicalInference, SIGNAL(dataChangedSignal()), m_sourceOntologyWidget, SLOT(dataChangedSlot()));
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
  QImage screenshot = m_sourceOntologyWidget->makeScreenshot();
  screenshot.save(filePath);
}

void MainWindow::currentTabChangedSlot(int index) {

  switch (index) {
    case 0:
      onSourceOntologyWidgetShow();
      break;

    case 1:
      onDestinationOntologyWidgetShow();
      break;

    case 2:
      onJavaOntologyWidgetShow();
      break;

    case 3:
      onObjcOntologyWidgetShow();
      break;

    case 4:
      onProblemsOntologyWidgetShow();
      break;
  }
}
