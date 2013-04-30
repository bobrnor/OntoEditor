#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidget>

#include "lib_json/json/json.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {

  ui->setupUi(this);

  setupSourceOntologyWidget();
  setupDestinationOntologyWidget();
  setupJavaOntologyWidget();
  setupObjcOntologyWidget();
  setupProblemsOntologyWidget();

  m_projectTreeViewController = new ProjectTreeViewController();
  m_projectTreeViewController->setProject(&m_currentProject);
  ui->projectTreeLayout->addWidget(m_projectTreeViewController->widget());

  m_ontologyTreeViewController = new OntologyTreeViewController();
  ui->ontologyTreeLayout->addWidget(m_ontologyTreeViewController->treeView());

  m_transformationHelper = new TransformationHelper();

  m_logTreeView = new QTreeView();
  m_logTreeView->setHeaderHidden(true);
  m_logTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_logTreeView->setModel(m_transformationHelper->logModel());
  m_logTreeView->hide();

  m_zoomInShortcut = new QShortcut(this);
  m_zoomInShortcut->setKey(QKeySequence("Ctrl+="));
  m_zoomInShortcut->setEnabled(true);

  m_zoomOutShortcut = new QShortcut(this);
  m_zoomOutShortcut->setKey(QKeySequence("Ctrl+-"));
  m_zoomOutShortcut->setEnabled(true);

  m_removeShortcut = new QShortcut(this);
  m_removeShortcut->setKey(QKeySequence::Delete);
  m_removeShortcut->setEnabled(true);

  QAction *toBeginAction = ui->toolBar->addAction(tr("Move to start"));
  connect(toBeginAction, SIGNAL(triggered()), SLOT(moveToStartSlot()));

  QAction *moveBackwardAction = ui->toolBar->addAction(tr("Move backward"));
  connect(moveBackwardAction, SIGNAL(triggered()), SLOT(moveBackwardSlot()));

  QAction *moveForwardAction = ui->toolBar->addAction(tr("Move forward"));
  connect(moveForwardAction, SIGNAL(triggered()), SLOT(moveForwardSlot()));

  QAction *toEndAction = ui->toolBar->addAction(tr("Move to the end"));
  connect(toEndAction, SIGNAL(triggered()), SLOT(moveToEndSlot()));

  m_currentSnapshotIndex = 0;

  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(currentTabChangedSlot(int)));
  connect(m_projectTreeViewController, SIGNAL(currentFileChangedSignal(QString)), SLOT(currentFileChangedSlot(QString)));
  connect(m_projectTreeViewController, SIGNAL(categorySelectedSignal(QString,QString)), SLOT(categorySelectedSlot(QString,QString)));

  setupMenu();
  onSourceOntologyWidgetShow();
}

MainWindow::~MainWindow() {

  delete ui;
}

void MainWindow::setupSourceOntologyWidget() {

  m_sourceOntologyWidget = new OntologyWidget(this);

  QWidget *dsTab = ui->tabWidget->widget(0);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  dsTab->setLayout(layout);
  layout->addWidget(m_sourceOntologyWidget);
}

void MainWindow::setupDestinationOntologyWidget() {

  m_destinationOntologyWidget = new OntologyWidget(this);

  QWidget *dsTab = ui->tabWidget->widget(1);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  dsTab->setLayout(layout);
  layout->addWidget(m_destinationOntologyWidget);
}

void MainWindow::setupJavaOntologyWidget() {

  m_javaOntologyWidget = new OntologyWidget();

  QWidget *javaTab = ui->tabWidget->widget(2);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  javaTab->setLayout(layout);
  layout->addWidget(m_javaOntologyWidget);
}

void MainWindow::setupObjcOntologyWidget() {

  m_objcOntologyWidget = new OntologyWidget(this);

  QWidget *objcTab = ui->tabWidget->widget(3);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  objcTab->setLayout(layout);
  layout->addWidget(m_objcOntologyWidget);
}

void MainWindow::setupProblemsOntologyWidget() {

  m_problemsOntologyWidget = new OntologyWidget(this);

  QWidget *problemsTab = ui->tabWidget->widget(4);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0);
  problemsTab->setLayout(layout);
  layout->addWidget(m_problemsOntologyWidget);
}

void MainWindow::onSourceOntologyWidgetShow() {

  clearConnections();

  if (m_currentFileName.isEmpty()) {
    m_ontologyTreeViewController->setDataController(NULL);
  }
  else {
    const ProjectFile *file = m_currentProject.getProjectFileByName(m_currentFileName);
    m_ontologyTreeViewController->setDataController(file->sourceOntologyController());
  }

  connect(this, SIGNAL(showOntologyGraphSignal()), m_sourceOntologyWidget, SLOT(showOntologySlot()));
  connect(this, SIGNAL(showSourceCodeSignal()), m_sourceOntologyWidget, SLOT(showSourceCodeSlot()));

  connect(m_sourceOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_sourceOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_sourceOntologyWidget, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));

  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_sourceOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_sourceOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_sourceOntologyWidget, SLOT(removeSelectedSlot()));

  connect(this, SIGNAL(itemsSelectedSignal(QSet<long>)), m_sourceOntologyWidget, SLOT(itemsSelectedSlot(QSet<long>)));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(true);
  m_sourceOntologyWidget->updateData();
}

void MainWindow::onDestinationOntologyWidgetShow() {

  clearConnections();

  if (m_currentFileName.isEmpty()) {
    m_ontologyTreeViewController->setDataController(NULL);
  }
  else {
    const ProjectFile *file = m_currentProject.getProjectFileByName(m_currentFileName);
    m_ontologyTreeViewController->setDataController(file->destinationOntologyController());
  }

  connect(this, SIGNAL(showOntologyGraphSignal()), m_destinationOntologyWidget, SLOT(showOntologySlot()));
  connect(this, SIGNAL(showSourceCodeSignal()), m_destinationOntologyWidget, SLOT(showSourceCodeSlot()));

  connect(m_destinationOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_destinationOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_destinationOntologyWidget, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));

  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_destinationOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_destinationOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_destinationOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(false);
  m_destinationOntologyWidget->updateData();
}

void MainWindow::onJavaOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataController(m_currentProject.getLanguageOntologyByName("java"));

  connect(m_javaOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_javaOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_javaOntologyWidget, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));

  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_javaOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_javaOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_javaOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(false);
  m_javaOntologyWidget->updateData();
}

void MainWindow::onObjcOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataController(m_currentProject.getLanguageOntologyByName("objc"));

  connect(m_objcOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_objcOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_objcOntologyWidget, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));

  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_objcOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_objcOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_objcOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(false);
  m_objcOntologyWidget->updateData();
}

void MainWindow::onProblemsOntologyWidgetShow() {

  clearConnections();

  m_ontologyTreeViewController->setDataController(m_currentProject.problemsOntologyController());

  connect(m_problemsOntologyWidget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_problemsOntologyWidget, SLOT(dataChangedSlot()));

  connect(m_problemsOntologyWidget, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), m_transformationHelper, SLOT(dataChangedSlot()));

  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_problemsOntologyWidget, SLOT(dataChangedSlot()));
  connect(m_transformationHelper, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), m_problemsOntologyWidget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), m_problemsOntologyWidget, SLOT(removeSelectedSlot()));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(false);
  m_problemsOntologyWidget->updateData();
}

void MainWindow::clearConnections() {

  m_problemsOntologyWidget->disconnect();
  m_ontologyTreeViewController->disconnect();
  m_transformationHelper->disconnect();
  m_zoomInShortcut->disconnect();
  m_zoomOutShortcut->disconnect();
  m_removeShortcut->disconnect();
}

void MainWindow::updateOntologyTreeData() {

  int index = ui->tabWidget->currentIndex();
  switch (index) {
    case 0:
      if (m_currentFileName.isEmpty()) {
        m_ontologyTreeViewController->setDataController(NULL);
      }
      else {
        const ProjectFile *file = m_currentProject.getProjectFileByName(m_currentFileName);
        m_ontologyTreeViewController->setDataController(file->sourceOntologyController());
      }
      break;

    case 1:
      if (m_currentFileName.isEmpty()) {
        m_ontologyTreeViewController->setDataController(NULL);
      }
      else {
        const ProjectFile *file = m_currentProject.getProjectFileByName(m_currentFileName);
        m_ontologyTreeViewController->setDataController(file->destinationOntologyController());
      }
      break;

    case 2:
      m_ontologyTreeViewController->setDataController(m_currentProject.getLanguageOntologyByName("java"));
      break;

    case 3:
      m_ontologyTreeViewController->setDataController(m_currentProject.getLanguageOntologyByName("objc"));
      break;

    case 4:
      m_ontologyTreeViewController->setDataController(m_currentProject.problemsOntologyController());
      break;
  }

  m_ontologyTreeViewController->updateData();
}

void MainWindow::setupMenu() {

  QMenu *fileMenu = ui->menubar->addMenu(tr("File"));

  QAction *importSourceFileAction = fileMenu->addAction(tr("Import source file..."));
  QAction *exportGeneratedFileAction = fileMenu->addAction(tr("Export generated file..."));

  fileMenu->addSeparator();

  QAction *openWorkspaceAction = fileMenu->addAction(tr("Open workspace..."));
  QAction *saveWorkspaceAction = fileMenu->addAction(tr("Save workspace..."));

  fileMenu->addSeparator();

  QAction *openProjectAction = fileMenu->addAction(tr("Open project..."));
  QAction *saveProjectAction = fileMenu->addAction(tr("Save project..."));

  fileMenu->addSeparator();

  QAction *screenshotAction = fileMenu->addAction(tr("Screenshot..."));

  QMenu *viewMenu = ui->menubar->addMenu(tr("View"));

  QAction *ontologyGraphAction = viewMenu->addAction(tr("Show ontology graph"));
  QAction *sourceCodeAction = viewMenu->addAction(tr("Show source code"));

  QMenu *transformationMenu = ui->menubar->addMenu(tr("Transformation"));

  QAction *transformAction = transformationMenu->addAction(tr("Transform"));

  connect(importSourceFileAction, SIGNAL(triggered()), SLOT(importSourceFileSlot()));
  connect(exportGeneratedFileAction, SIGNAL(triggered()), SLOT(exportGeneratedFileSlot()));

  connect(openWorkspaceAction, SIGNAL(triggered()), SLOT(openWorkspaceSlot()));
  connect(saveWorkspaceAction, SIGNAL(triggered()), SLOT(saveWorkspaceSlot()));

  connect(openProjectAction, SIGNAL(triggered()), SLOT(openProjectSlot()));
  connect(saveProjectAction, SIGNAL(triggered()), SLOT(saveProjectSlot()));

  connect(ontologyGraphAction, SIGNAL(triggered()), SIGNAL(showOntologyGraphSignal()));
  connect(sourceCodeAction, SIGNAL(triggered()), SIGNAL(showSourceCodeSignal()));

  connect(screenshotAction, SIGNAL(triggered()), SLOT(screenshotSlot()));

  connect(transformAction, SIGNAL(triggered()), SLOT(transformSlot()));
}

void MainWindow::importSourceFileSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*");
  bool result = m_currentProject.importSourceFile(filePath);

  if (result) {
    m_sourceOntologyWidget->updateData();
    updateOntologyTreeData();
    m_projectTreeViewController->updateData();
  }
}

void MainWindow::exportGeneratedFileSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*");
  m_currentProject.exportDestinationFile(m_currentFileName, filePath);
}

void MainWindow::openWorkspaceSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*.ojs");
  bool result = m_currentProject.openWorkspace(filePath);

  if (result) {
    m_javaOntologyWidget->setDataController(m_currentProject.getLanguageOntologyByName("java"));
    m_javaOntologyWidget->dataChangedSlot();

    m_objcOntologyWidget->setDataController(m_currentProject.getLanguageOntologyByName("objc"));
    m_objcOntologyWidget->dataChangedSlot();

    m_problemsOntologyWidget->setDataController(m_currentProject.problemsOntologyController());
    m_problemsOntologyWidget->dataChangedSlot();

    updateOntologyTreeData();    
  }
}

void MainWindow::saveWorkspaceSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.ojs");
  m_currentProject.saveWorkspace(filePath);
}

void MainWindow::openProjectSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*.pjs");
  bool result = m_currentProject.openProject(filePath);

  if (result) {
    m_javaOntologyWidget->setDataController(m_currentProject.getLanguageOntologyByName("java"));
    m_javaOntologyWidget->dataChangedSlot();

    m_objcOntologyWidget->setDataController(m_currentProject.getLanguageOntologyByName("objc"));
    m_objcOntologyWidget->dataChangedSlot();

    m_problemsOntologyWidget->setDataController(m_currentProject.problemsOntologyController());
    m_problemsOntologyWidget->dataChangedSlot();

    updateOntologyTreeData();
    m_sourceOntologyWidget->updateData();
    m_projectTreeViewController->updateData();
  }
}

void MainWindow::saveProjectSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.pjs");
  m_currentProject.saveProject(filePath);
}

void MainWindow::screenshotSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.png");
  QImage screenshot;
  switch (ui->tabWidget->currentIndex()) {
    case 0:
      screenshot = m_sourceOntologyWidget->makeScreenshot();
      break;

    case 1:
      screenshot = m_destinationOntologyWidget->makeScreenshot();
      break;

    case 2:
      screenshot = m_javaOntologyWidget->makeScreenshot();
      break;

    case 3:
      screenshot = m_objcOntologyWidget->makeScreenshot();
      break;

    case 4:
      screenshot = m_problemsOntologyWidget->makeScreenshot();
      break;
  }
  screenshot.save(filePath);
}

void MainWindow::transformSlot() {

  moveToEndSlot();
  m_transformationHelper->process();

  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);
  if (currentFile != NULL) {
    qDebug() << m_currentFileName;
    QString code = m_currentProject.generateDestinationCode(m_currentFileName);
    qDebug() << "Generated code: " << code;
    currentFile->destinationOntologyController()->setSourceCode(code);
    currentFile->destinationOntologyController()->normalize();
  }

  m_currentSnapshotIndex = m_transformationHelper->snapshots().count();

  m_logTreeView->expandAll();
  m_logTreeView->showNormal();
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

void MainWindow::currentFileChangedSlot(const QString &fileName) {

  ProjectFile *file = m_currentProject.getProjectFileByName(fileName);
  if (file != NULL) {
    m_currentFileName = fileName;

    m_transformationHelper->setSourceOntology(file->sourceOntologyController());
    m_transformationHelper->setDestinationOntology(file->destinationOntologyController());
    m_transformationHelper->setProblemsOntology(m_currentProject.problemsOntologyController());

    m_sourceOntologyWidget->setDataController(file->sourceOntologyController());
    m_sourceOntologyWidget->dataChangedSlot();

    m_destinationOntologyWidget->setDataController(file->destinationOntologyController());
    m_destinationOntologyWidget->dataChangedSlot();
  }
}

void MainWindow::categorySelectedSlot(const QString &fileName, const QString &categoryName) {

  ProjectFile *file = m_currentProject.getProjectFileByName(fileName);
  if (file != NULL) {
    if (m_currentFileName != fileName) {
      m_currentFileName = fileName;

      m_transformationHelper->setSourceOntology(file->sourceOntologyController());
      m_transformationHelper->setDestinationOntology(file->destinationOntologyController());
      m_transformationHelper->setProblemsOntology(m_currentProject.problemsOntologyController());

      m_sourceOntologyWidget->setDataController(file->sourceOntologyController());
      m_sourceOntologyWidget->dataChangedSlot();

      m_destinationOntologyWidget->setDataController(file->destinationOntologyController());
      m_destinationOntologyWidget->dataChangedSlot();
    }

    ProjectFileCategory *category = file->getCategoryByName(categoryName);
    if (category != NULL) {
      emit itemsSelectedSignal(category->relatedNodeIds());
    }
  }
}

void MainWindow::moveToStartSlot() {

  qDebug() << "Move to start";

  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

  if (currentFile != NULL) {
    if (m_currentSnapshotIndex > 0)  {
      m_currentSnapshotIndex = 0;

      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

      m_sourceOntologyWidget->setDataController(sourceDataController);
      m_sourceOntologyWidget->dataChangedSlot();

      m_destinationOntologyWidget->setDataController(destinationDataController);
      m_destinationOntologyWidget->dataChangedSlot();

      m_problemsOntologyWidget->setDataController(problemsDataController);
      m_problemsOntologyWidget->dataChangedSlot();

      m_logTreeView->setModel(snapshot->logModelSnapshot());
      m_logTreeView->expandAll();

      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(0);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
      else {
        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(1);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
    }
  }
}

void MainWindow::moveForwardSlot() {

  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

  if (currentFile != NULL) {
    if (m_currentSnapshotIndex < m_transformationHelper->snapshots().count() - 1) {
      m_currentSnapshotIndex++;

      qDebug() << "Move to " << m_currentSnapshotIndex << " snapshot";

      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

      m_sourceOntologyWidget->setDataController(sourceDataController);
      m_sourceOntologyWidget->dataChangedSlot();

      m_destinationOntologyWidget->setDataController(destinationDataController);
      m_destinationOntologyWidget->dataChangedSlot();

      m_problemsOntologyWidget->setDataController(problemsDataController);
      m_problemsOntologyWidget->dataChangedSlot();

      m_logTreeView->setModel(snapshot->logModelSnapshot());
      m_logTreeView->expandAll();

      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(0);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
      else {
        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(1);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
    }
    else {
      moveToEndSlot();
    }
  }
}

void MainWindow::moveBackwardSlot() {

  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

  if (currentFile != NULL) {
    if (m_currentSnapshotIndex > 0) {
      m_currentSnapshotIndex--;

      qDebug() << "Move to " << m_currentSnapshotIndex << " snapshot";

      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

      m_sourceOntologyWidget->setDataController(sourceDataController);
      m_sourceOntologyWidget->dataChangedSlot();

      m_destinationOntologyWidget->setDataController(destinationDataController);
      m_destinationOntologyWidget->dataChangedSlot();

      m_problemsOntologyWidget->setDataController(problemsDataController);
      m_problemsOntologyWidget->dataChangedSlot();

      m_logTreeView->setModel(snapshot->logModelSnapshot());
      m_logTreeView->expandAll();

      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(0);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
      else {
        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
          ui->tabWidget->setCurrentIndex(1);
        }
        else {
          ui->tabWidget->setCurrentIndex(4);
        }
      }
    }
  }
}

void MainWindow::moveToEndSlot() {

  qDebug() << "Move to the end";

  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);
  if (m_currentFileName != NULL) {
    m_currentSnapshotIndex = m_transformationHelper->snapshots().count();

    m_sourceOntologyWidget->setDataController(currentFile->sourceOntologyController());
    m_sourceOntologyWidget->dataChangedSlot();

    m_destinationOntologyWidget->setDataController(currentFile->destinationOntologyController());
    m_destinationOntologyWidget->dataChangedSlot();

    m_problemsOntologyWidget->setDataController(m_currentProject.problemsOntologyController());
    m_problemsOntologyWidget->dataChangedSlot();

    m_logTreeView->setModel(m_transformationHelper->logModel());
    m_logTreeView->expandAll();

    ui->tabWidget->setCurrentIndex(1);
  }
}
