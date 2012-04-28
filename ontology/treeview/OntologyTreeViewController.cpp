#include "OntologyTreeViewController.h"

OntologyTreeViewController::OntologyTreeViewController() {

  m_objectsTreeView = new QTreeView();
  m_objectsTreeView->setHeaderHidden(true);
  m_objectsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  m_objectsModel = new QStandardItemModel(2, 1);
  m_objectsTreeView->setModel(m_objectsModel);
  m_dataSource = NULL;
  m_delegate = NULL;

  QModelIndex nodeIndex = m_objectsModel->index(0, 0);
  m_objectsModel->insertColumn(0, nodeIndex);
  m_objectsModel->setData(nodeIndex, tr("Nodes"));

  QModelIndex relationIndex = m_objectsModel->index(1, 0);
  m_objectsModel->insertColumn(0, relationIndex);
  m_objectsModel->setData(relationIndex, tr("Relations"));

  connect(m_objectsTreeView, SIGNAL(activated(QModelIndex)), SLOT(itemSelectedSlot(QModelIndex)));
}

OntologyTreeViewController::~OntologyTreeViewController() {

  delete m_objectsTreeView;
  delete m_objectsModel;
}

QTreeView *OntologyTreeViewController::treeView() const {

  return m_objectsTreeView;
}

void OntologyTreeViewController::setDataSource(IOntologyDataSource *dataSource) {

  m_dataSource = dataSource;
}

IOntologyDataSource *OntologyTreeViewController::dataSource() const {

  return m_dataSource;
}

void OntologyTreeViewController::setDelegate(IOntologyDelegate *delegate) {

  m_delegate = delegate;
}

IOntologyDelegate *OntologyTreeViewController::delegate() const {

  return m_delegate;
}

void OntologyTreeViewController::updateData() {

  if (m_dataSource != NULL) {
    QModelIndex nodesIndex = m_objectsModel->index(0, 0);
    int currentNodeRowsCount = m_objectsModel->rowCount(nodesIndex);
    int nodesCount = m_dataSource->nodeCount();

    int countDiff = currentNodeRowsCount - nodesCount;
    if (countDiff > 0) {
      m_objectsModel->removeRows(0, countDiff, nodesIndex);
    }
    else if (countDiff < 0) {
      m_objectsModel->insertRows(0, abs(countDiff), nodesIndex);
    }

    for (int i = 0; i < nodesCount; ++i) {
      QModelIndex index = m_objectsModel->index(i, 0, nodesIndex);
      NodeData *nodeData = m_dataSource->node(i);
      m_objectsModel->setData(index, nodeData->name + "[" + QString::number(nodeData->id) + "]");
      m_objectsModel->setData(index, (qlonglong)nodeData->id, Qt::UserRole);
    }

    QModelIndex relationsIndex = m_objectsModel->index(1, 0);
    int currentRelationRowsCount = m_objectsModel->rowCount(relationsIndex);
    int relationsCount = m_dataSource->relationCount();

    countDiff = currentRelationRowsCount - relationsCount;
    if (countDiff > 0) {
      m_objectsModel->removeRows(0, countDiff, relationsIndex);
    }
    else if (countDiff < 0) {
      m_objectsModel->insertRows(0, abs(countDiff), relationsIndex);
    }

    for (int i = 0; i < relationsCount; ++i) {
      QModelIndex index = m_objectsModel->index(i, 0, relationsIndex);
      RelationData *relationData = m_dataSource->relation(i);
      m_objectsModel->setData(index, relationData->name + "[" + QString::number(relationData->id) + "]");
      m_objectsModel->setData(index, (qlonglong)relationData->id, Qt::UserRole);
    }
  }
}

void OntologyTreeViewController::dataChangedSlot() {

  updateData();
}

void OntologyTreeViewController::itemSelectedSlot(const QModelIndex &index) {

  long id = m_objectsModel->data(index, Qt::UserRole).toLongLong();
  if (id > 0) {
    emit itemSelectedSignal(id);
  }
}
