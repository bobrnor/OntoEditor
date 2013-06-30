#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {

  ui->setupUi(this);

  m_projectTreeViewController = new ProjectTreeViewController();
  m_projectTreeViewController->setProject(&m_currentProject);
  ui->projectTreeLayout->addWidget(m_projectTreeViewController->widget());

  m_ontologyTreeViewController = new OntologyTreeViewController();
  ui->ontologyTreeLayout->addWidget(m_ontologyTreeViewController->treeView());

  m_logTreeView = new QTreeView();
  m_logTreeView->setHeaderHidden(true);
  m_logTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//  m_logTreeView->setModel(m_transformationHelper->logModel());
  m_logTreeView->hide();

  m_palleteWidget = new PalleteWidget();
  ui->palleteLayout->addWidget(m_palleteWidget);

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
  setupConverters();
  setupTransformators();
}

MainWindow::~MainWindow() {

  delete ui;
}

OntologyWidget * MainWindow::createNewOntologyWidget(ProjectFile *file) {

  OntologyWidget *newWidget = new OntologyWidget(this);
  newWidget->setDataController(file->ontologyController());
  m_openOntologyWidgets.append(newWidget);
  ui->tabWidget->addTab(newWidget, file->name());

  return newWidget;
}

void MainWindow::clearConnections() {

  m_ontologyTreeViewController->disconnect();
  m_zoomInShortcut->disconnect();
  m_zoomOutShortcut->disconnect();
  m_removeShortcut->disconnect();
}

void MainWindow::updateOntologyTreeData() {

  int index = ui->tabWidget->currentIndex();
  OntologyWidget *widget = m_openOntologyWidgets[index];
  m_ontologyTreeViewController->setDataController(widget->dataController());
  m_ontologyTreeViewController->updateData();
}

void MainWindow::setupConverters() {

  QDir dir("/Users/bobrnor/Dropbox/PSU/Projects/OntoEditor/scripts");

  foreach (QString fileName, dir.entryList(QDir::Files)) {
    if (fileName.contains("converter")) {
      QString cmd;
      cmd.append("/Library/Frameworks/Python.framework/Versions/2.7/bin/python");
      cmd.append(" " + dir.absoluteFilePath(fileName));
      cmd.append(" --method=supported_extensions");

      FILE* pipe = popen(cmd.toStdString().c_str(), "r");

      char buffer[128];
      QString result = "";
      while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
          result += buffer;
      }
      pclose(pipe);

      qDebug() << result;

      QStringList extensions = result.split(":");
      foreach (QString extension, extensions) {
        m_convertersMapping.insert(extension.trimmed(), dir.absoluteFilePath(fileName));
      }
    }
  }
}

void MainWindow::setupTransformators() {

  // do nothing now
}

void MainWindow::setupMenu() {

  QMenu *fileMenu = ui->menubar->addMenu(tr("File"));

  QAction *importSourceFileAction = fileMenu->addAction(tr("Import source file..."));

  fileMenu->addSeparator();

  QAction *createOntologyFileAction = fileMenu->addAction(tr("Create ontology..."));
  QAction *openOntologyFileAction = fileMenu->addAction(tr("Open ontology..."));
  QAction *saveOntologyFileAction = fileMenu->addAction(tr("Save ontology..."));

  fileMenu->addSeparator();

  QAction *openProjectAction = fileMenu->addAction(tr("Open project..."));
  QAction *saveProjectAction = fileMenu->addAction(tr("Save project..."));

  fileMenu->addSeparator();

  QAction *exportFileAction = fileMenu->addAction(tr("Export file..."));

  fileMenu->addSeparator();

  QAction *screenshotAction = fileMenu->addAction(tr("Screenshot..."));

  QMenu *transformationMenu = ui->menubar->addMenu(tr("Transformation"));

  QAction *transformAction = transformationMenu->addAction(tr("Transform"));

  connect(importSourceFileAction, SIGNAL(triggered()), SLOT(importSourceFileSlot()));

  connect(createOntologyFileAction, SIGNAL(triggered()), SLOT(createOntologyFileSlot()));
  connect(openOntologyFileAction, SIGNAL(triggered()), SLOT(openOntologyFileSlot()));
  connect(saveOntologyFileAction, SIGNAL(triggered()), SLOT(saveOntologyFileSlot()));

  connect(openProjectAction, SIGNAL(triggered()), SLOT(openProjectSlot()));
  connect(saveProjectAction, SIGNAL(triggered()), SLOT(saveProjectSlot()));

  connect(exportFileAction, SIGNAL(triggered()), SLOT(exportFileSlot()));

  connect(screenshotAction, SIGNAL(triggered()), SLOT(screenshotSlot()));

  connect(transformAction, SIGNAL(triggered()), SLOT(transformSlot()));
}

void MainWindow::importSourceFileSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*");

  QFileInfo info(filePath);
  QString converterPath = m_convertersMapping["." + info.completeSuffix()];

  if (converterPath.length() > 0) {
    QString cmd;
    cmd.append("/Library/Frameworks/Python.framework/Versions/2.7/bin/python");
    cmd.append(" " + converterPath);
    cmd.append(" --method=import");
    cmd.append(" --source-path=");
    cmd.append(filePath);

    FILE* pipe = popen(cmd.toStdString().c_str(), "r");

    char buffer[128];
    QString result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
    pclose(pipe);

    ProjectFile *file = m_currentProject.createFile(result);
    if (file != NULL) {
      OntologyWidget *widget = createNewOntologyWidget(file);
      widget->dataChangedSlot();

      updateOntologyTreeData();
      m_projectTreeViewController->updateData();
    }
  }
}

void MainWindow::createOntologyFileSlot() {

  ProjectFile *file = m_currentProject.createFile();
  if (file != NULL) {
    OntologyWidget *widget = createNewOntologyWidget(file);
    widget->dataChangedSlot();

    updateOntologyTreeData();
    m_projectTreeViewController->updateData();
  }
}

void MainWindow::openOntologyFileSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*");
  ProjectFile *file = m_currentProject.openFile(filePath);

  if (file != NULL) {
    OntologyWidget *widget = createNewOntologyWidget(file);
    widget->dataChangedSlot();

    updateOntologyTreeData();
    m_projectTreeViewController->updateData();
  }
}

void MainWindow::saveOntologyFileSlot() {

  int index = ui->tabWidget->currentIndex();
  ProjectFile *file = m_currentProject.getProjectFileByIndex(index);
  if (file->path().length() > 0) {
    m_currentProject.saveFile(file);
  }
  else {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Open dialog"), QString(), "*");
    m_currentProject.saveFile(file, filePath);
  }
}

void MainWindow::openProjectSlot() {

  QString filePath = QFileDialog::getOpenFileName(this, tr("Open dialog"), QString(), "*.pjs");
  bool result = m_currentProject.openProject(filePath);

  if (result) {
    for (int i = 0; i < m_currentProject.filesCount(); ++i) {
      ProjectFile *file = m_currentProject.getProjectFileByIndex(i);

      OntologyWidget *widget = createNewOntologyWidget(file);
      widget->dataChangedSlot();
    }

    updateOntologyTreeData();
    m_projectTreeViewController->updateData();
  }
}

void MainWindow::saveProjectSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.pjs");
  m_currentProject.saveProject(filePath);
}

void MainWindow::exportFileSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Open dialog"), QString(), "*");

  QFileInfo info(filePath);
  QString converterPath = m_convertersMapping["." + info.completeSuffix()];

  if (converterPath.length() > 0) {
    int index = ui->tabWidget->currentIndex();
    ProjectFile *file = m_currentProject.getProjectFileByIndex(index);

    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    tmpFile.open();
    QVariant json = file->ontologyController()->serialize();
    QJson::Serializer serializer;
    QByteArray data = serializer.serialize(json);
    tmpFile.write(data);
    tmpFile.flush();

    QString cmd;
    cmd.append("/Library/Frameworks/Python.framework/Versions/2.7/bin/python");
    cmd.append(" " + converterPath);
    cmd.append(" --method=export");
    cmd.append(" --source-path=");
    cmd.append(tmpFile.fileName());

    FILE* pipe = popen(cmd.toStdString().c_str(), "r");

    char buffer[128];
    QString result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
    pclose(pipe);

    QFile dstFile(filePath);
    dstFile.open(QIODevice::WriteOnly | QIODevice::Text);
    dstFile.write(result.toLocal8Bit());
  }
}

void MainWindow::screenshotSlot() {

  QString filePath = QFileDialog::getSaveFileName(this, tr("Save dialog"), QString(), "*.png");
  OntologyWidget *widget = m_openOntologyWidgets[ui->tabWidget->currentIndex()];
  QImage screenshot = widget->makeScreenshot();
  screenshot.save(filePath);
}

void MainWindow::currentTabChangedSlot(int index) {

  OntologyWidget *widget = m_openOntologyWidgets[index];

  clearConnections();

  m_ontologyTreeViewController->setDataController(widget->dataController());

  connect(widget, SIGNAL(dataChangedSignal()), m_ontologyTreeViewController, SLOT(dataChangedSlot()));
  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), widget, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(dataChangedSignal()), widget, SLOT(dataChangedSlot()));

  connect(m_ontologyTreeViewController, SIGNAL(itemSelectedSignal(long)), widget, SLOT(itemSelectedSlot(long)));

  connect(m_zoomInShortcut, SIGNAL(activated()), widget, SLOT(zoomInSlot()));
  connect(m_zoomOutShortcut, SIGNAL(activated()), widget, SLOT(zoomOutSlot()));
  connect(m_removeShortcut, SIGNAL(activated()), widget, SLOT(removeSelectedSlot()));

  connect(this, SIGNAL(itemsSelectedSignal(QSet<long>)), widget, SLOT(itemsSelectedSlot(QSet<long>)));

  m_ontologyTreeViewController->updateData();
  m_ontologyTreeViewController->setDragEnabled(true);
  widget->updateData();
}

void MainWindow::currentFileChangedSlot(const QString &fileName) {

  for (int i = 0; i < m_currentProject.filesCount(); ++i) {
    ProjectFile *file = m_currentProject.getProjectFileByIndex(i);
    if (file->name() == fileName) {
      ui->tabWidget->setCurrentIndex(i);
    }
  }
}

void MainWindow::categorySelectedSlot(const QString &fileName, const QString &categoryName) {

  ProjectFile *file = NULL;

  for (int i = 0; i < m_currentProject.filesCount(); ++i) {
    ProjectFile *projFile = m_currentProject.getProjectFileByIndex(i);
    if (projFile->name() == fileName) {
      file = projFile;
      break;
    }
  }

  if (file != NULL) {
    ProjectFileCategory *category = file->getCategoryByName(categoryName);
    if (category != NULL) {
      emit itemsSelectedSignal(category->relatedNodeIds());
    }
  }
}

void MainWindow::transformSlot() {

}

void MainWindow::moveToStartSlot() {

  qDebug() << "Move to start";

//  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

//  if (currentFile != NULL) {
//    if (m_currentSnapshotIndex > 0)  {
//      m_currentSnapshotIndex = 0;

//      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

//      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
//      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
//      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

//      m_sourceOntologyWidget->setDataController(sourceDataController);
//      m_sourceOntologyWidget->dataChangedSlot();

//      m_destinationOntologyWidget->setDataController(destinationDataController);
//      m_destinationOntologyWidget->dataChangedSlot();

//      m_problemsOntologyWidget->setDataController(problemsDataController);
//      m_problemsOntologyWidget->dataChangedSlot();

//      m_logTreeView->setModel(snapshot->logModelSnapshot());
//      m_logTreeView->expandAll();

//      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
//        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(0);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//      else {
//        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(1);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//    }
//  }
}

void MainWindow::moveForwardSlot() {

//  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

//  if (currentFile != NULL) {
//    if (m_currentSnapshotIndex < m_transformationHelper->snapshots().count() - 1) {
//      m_currentSnapshotIndex++;

//      qDebug() << "Move to " << m_currentSnapshotIndex << " snapshot";

//      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

//      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
//      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
//      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

//      m_sourceOntologyWidget->setDataController(sourceDataController);
//      m_sourceOntologyWidget->dataChangedSlot();

//      m_destinationOntologyWidget->setDataController(destinationDataController);
//      m_destinationOntologyWidget->dataChangedSlot();

//      m_problemsOntologyWidget->setDataController(problemsDataController);
//      m_problemsOntologyWidget->dataChangedSlot();

//      m_logTreeView->setModel(snapshot->logModelSnapshot());
//      m_logTreeView->expandAll();

//      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
//        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(0);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//      else {
//        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(1);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//    }
//    else {
//      moveToEndSlot();
//    }
//  }
}

void MainWindow::moveBackwardSlot() {

//  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);

//  if (currentFile != NULL) {
//    if (m_currentSnapshotIndex > 0) {
//      m_currentSnapshotIndex--;

//      qDebug() << "Move to " << m_currentSnapshotIndex << " snapshot";

//      Snapshot *snapshot = m_transformationHelper->snapshots().at(m_currentSnapshotIndex);

//      OntologyDataController *sourceDataController = snapshot->sourceOntologySnapshot();
//      OntologyDataController *destinationDataController = snapshot->destinationOntologySnapshot();
//      OntologyDataController *problemsDataController = snapshot->problemsOntologySnapshot();

//      m_sourceOntologyWidget->setDataController(sourceDataController);
//      m_sourceOntologyWidget->dataChangedSlot();

//      m_destinationOntologyWidget->setDataController(destinationDataController);
//      m_destinationOntologyWidget->dataChangedSlot();

//      m_problemsOntologyWidget->setDataController(problemsDataController);
//      m_problemsOntologyWidget->dataChangedSlot();

//      m_logTreeView->setModel(snapshot->logModelSnapshot());
//      m_logTreeView->expandAll();

//      if (sourceDataController->changesCount() > destinationDataController->changesCount()) {
//        if (sourceDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(0);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//      else {
//        if (destinationDataController->changesCount() > problemsDataController->changesCount()) {
//          ui->tabWidget->setCurrentIndex(1);
//        }
//        else {
//          ui->tabWidget->setCurrentIndex(4);
//        }
//      }
//    }
//  }
}

void MainWindow::moveToEndSlot() {

  qDebug() << "Move to the end";

//  ProjectFile *currentFile = m_currentProject.getProjectFileByName(m_currentFileName);
//  if (m_currentFileName != NULL) {
//    m_currentSnapshotIndex = m_transformationHelper->snapshots().count();

//    m_sourceOntologyWidget->setDataController(currentFile->sourceOntologyController());
//    m_sourceOntologyWidget->dataChangedSlot();

//    m_destinationOntologyWidget->setDataController(currentFile->destinationOntologyController());
//    m_destinationOntologyWidget->dataChangedSlot();

//    m_problemsOntologyWidget->setDataController(m_currentProject.problemsOntologyController());
//    m_problemsOntologyWidget->dataChangedSlot();

//    m_logTreeView->setModel(m_transformationHelper->logModel());
//    m_logTreeView->expandAll();

//    ui->tabWidget->setCurrentIndex(1);
//  }
}
