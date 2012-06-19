#include "ProjectTreeViewController.h"

#include <QDebug>
#include <QInputDialog>

#include "Project.h"
#include "ProjectFileCategory.h"

ProjectTreeViewController::ProjectTreeViewController() {

  m_treeWidget = new ProjectTreeWidget();
  m_treeWidget->setDelegate(this);
  m_treeWidget->setHeaderHidden(true);
  m_treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_treeWidget->setAcceptDrops(true);
  m_treeWidget->setDropIndicatorShown(true);

  setupMenu();

  connect(m_treeWidget, SIGNAL(activated(QModelIndex)), SLOT(itemSelectedSlot(QModelIndex)));
}

ProjectTreeViewController::~ProjectTreeViewController() {

  delete m_treeWidget;
}

void ProjectTreeViewController::setupMenu() {

  QAction *addCategoryAction = new QAction("Add category", m_treeWidget);
  connect(addCategoryAction, SIGNAL(triggered()), SLOT(addCategorySlot()));
  m_treeWidget->addAction(addCategoryAction);
}

void ProjectTreeViewController::setProject(Project *project) {

  m_project = project;
}

Project *ProjectTreeViewController::project() const {

  return m_project;
}

QTreeWidget *ProjectTreeViewController::widget() const {

  return m_treeWidget;
}

void ProjectTreeViewController::updateData() {

  m_treeWidget->clear();

  if (m_project != NULL) {
    m_treeWidget->setColumnCount(1);

    QList<QString> fileNames = m_project->availableFileNames();
    foreach (QString fileName, fileNames) {
      QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget, QStringList(fileName));
      item->setData(0, Qt::UserRole, QVariant(fileName));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

      ProjectFile *file = m_project->getProjectFileByName(fileName);
      if (file->categories().count() > 0) {
        foreach (ProjectFileCategory *category, file->categories()) {
          QTreeWidgetItem *categoryItem = new QTreeWidgetItem(item, QStringList(category->name()));
          categoryItem->setData(0, Qt::UserRole, QVariant(fileName));
          categoryItem->setData(0, Qt::UserRole + 1, QVariant(category->name()));
          categoryItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);

          if (category->relatedNodeIds().count() > 0) {
            foreach (long nodeId, category->relatedNodeIds()) {
              NodeData *nodeData = file->sourceOntologyController()->getNodeById(nodeId);
              Q_ASSERT(nodeData != NULL);
              QTreeWidgetItem *nodeItem = new QTreeWidgetItem(categoryItem, QStringList(nodeData->name));
//              categoryItem->setData(0, Qt::UserRole, QVariant(fileName));
//              categoryItem->setData(1, Qt::UserRole, QVariant(category->name()));
              nodeItem->setData(0, Qt::UserRole, QVariant((qlonglong)nodeData->id));
              nodeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
          }
        }
      }
    }
  }
}

bool ProjectTreeViewController::isCategoryIndex(const QModelIndex &index) {

  QMap<int, QVariant> itemData = m_treeWidget->model()->itemData(index);
  QString fileName = itemData.value(Qt::UserRole).toString();
  QString categoryName = itemData.value(Qt::UserRole + 1).toString();

  ProjectFile *projectFile = m_project->getProjectFileByName(fileName);
  if (projectFile != NULL) {
    ProjectFileCategory *category = projectFile->getCategoryByName(categoryName);
    if (category != NULL) {
      return true;
    }
  }

  return false;
}

bool ProjectTreeViewController::isFileIndex(const QModelIndex &index) {

  return index.parent() == QModelIndex();
}

bool ProjectTreeViewController::processDrop(QTreeWidgetItem *parentItem, const QMap<int, QVariant> &dropData) {

  QString fileName = parentItem->data(0, Qt::UserRole).toString();
  QString categoryName = parentItem->data(0, Qt::UserRole + 1).toString();
  long nodeId = dropData.value(Qt::UserRole).toLongLong();

  ProjectFile *projectFile = m_project->getProjectFileByName(fileName);
  if (projectFile != NULL) {
    ProjectFileCategory *category = projectFile->getCategoryByName(categoryName);
    if (category != NULL) {
      return category->addRelatedNodeId(nodeId);
    }
  }

  return false;
}

void ProjectTreeViewController::itemSelectedSlot(const QModelIndex &index) {

  if (isCategoryIndex(index)) {
    QString fileName = m_treeWidget->model()->data(index, Qt::UserRole).toString();
    QString categoryName = m_treeWidget->model()->data(index, Qt::UserRole + 1).toString();
    emit categorySelectedSignal(fileName, categoryName);
  }
  else if (isFileIndex(index)) {
    QString fileName = m_treeWidget->model()->data(index, Qt::UserRole).toString();
    emit currentFileChangedSignal(fileName);
  }
}

void ProjectTreeViewController::addCategorySlot() {

  QList<QTreeWidgetItem *> selectedItems = m_treeWidget->selectedItems();
  if (selectedItems.count() > 0) {
    QTreeWidgetItem *selectedItem = selectedItems.at(0);
    if (m_treeWidget->indexOfTopLevelItem(selectedItem) != -1) {
      QString fileName = selectedItem->data(0, Qt::UserRole).toString();
      ProjectFile *file = m_project->getProjectFileByName(fileName);

      bool ok = false;
      QString categoryName = QInputDialog::getText(m_treeWidget,
                                                   tr("Enter category name"),
                                                   tr("Name: "),
                                                   QLineEdit::Normal,
                                                   QString(),
                                                   &ok);

      if (ok) {
        ProjectFileCategory *category = new ProjectFileCategory(categoryName, file);
        file->addCategory(category);
        updateData();
      }
    }
  }
}
