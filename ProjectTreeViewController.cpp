#include "ProjectTreeViewController.h"

#include <QDebug>

#include "Project.h"

ProjectTreeViewController::ProjectTreeViewController() {

  m_treeWidget = new QTreeWidget();
  m_treeWidget->setHeaderHidden(true);
  m_treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  connect(m_treeWidget, SIGNAL(activated(QModelIndex)), SLOT(itemSelectedSlot(QModelIndex)));
}

ProjectTreeViewController::~ProjectTreeViewController() {

  delete m_treeWidget;
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
    }
  }
}

void ProjectTreeViewController::itemSelectedSlot(const QModelIndex &index) {

  QString fileName = m_treeWidget->model()->data(index, Qt::UserRole).toString();
  qDebug() << "Show file: " << fileName;
  emit currentFileChangedSignal(fileName);
}
