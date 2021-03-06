#include "OntologyTreeViewController.h"

#include <QDebug>

struct BuildTreeQueueItem {
    QStandardItem *item;
    QList<TVNodeData> nodes;
};

OntologyTreeViewController::OntologyTreeViewController() {

  m_objectsTreeView = new QTreeView();
  m_objectsTreeView->setHeaderHidden(true);
  m_objectsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_objectsTreeView->setDragDropMode(QAbstractItemView::DragOnly);
  m_objectsTreeView->setDragEnabled(true);

  m_objectsModel = new QStandardItemModel();
  m_objectsTreeView->setModel(m_objectsModel);
  m_dataController = NULL;

  clearTreeView();

  connect(m_objectsTreeView, SIGNAL(activated(QModelIndex)), SLOT(itemSelectedSlot(QModelIndex)));
}

OntologyTreeViewController::~OntologyTreeViewController() {

  delete m_objectsTreeView;
  delete m_objectsModel;
}

void OntologyTreeViewController::clearTreeView() {

  m_objectsModel->clear();
  m_objectsModel->insertColumns(0, 1);
  m_objectsModel->insertRows(0, 3);

  QModelIndex nodeTreeIndex = m_objectsModel->index(0, 0);
  m_objectsModel->insertColumn(0, nodeTreeIndex);
  m_objectsModel->setData(nodeTreeIndex, tr("Tree"));
  m_objectsModel->itemFromIndex(nodeTreeIndex)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QModelIndex nodeIndex = m_objectsModel->index(1, 0);
  m_objectsModel->insertColumn(0, nodeIndex);
  m_objectsModel->setData(nodeIndex, tr("Nodes"));
  m_objectsModel->itemFromIndex(nodeIndex)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QModelIndex relationIndex = m_objectsModel->index(2, 0);
  m_objectsModel->insertColumn(0, relationIndex);
  m_objectsModel->setData(relationIndex, tr("Relations"));
  m_objectsModel->itemFromIndex(relationIndex)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

QTreeView *OntologyTreeViewController::treeView() const {

  return m_objectsTreeView;
}

void OntologyTreeViewController::setDataController(OntologyDataController *dataController) {

  m_dataController = dataController;
}

OntologyDataController *OntologyTreeViewController::dataController() const {

  return m_dataController;
}

void OntologyTreeViewController::setDragEnabled(bool enabled) {

  m_objectsTreeView->setDragEnabled(enabled);
}

bool OntologyTreeViewController::isDragEnabled() const {

  return m_objectsTreeView->dragEnabled();
}

void OntologyTreeViewController::updateTreeDataBottomToTop() {

  m_treeData.clear();

  if (m_dataController != NULL && m_dataController != NULL) {
    QMap<long, int> parentCounts;

    int nodesCount = m_dataController->nodeCount();
    for (int i = 0; i < nodesCount; ++i) {
      NodeData *nodeData = m_dataController->getNodeByIndex(i);
      TVNodeData tvNodeData;
      tvNodeData.nodeData = nodeData;
      // FIX: leak
      tvNodeData.childNodes = new QMap<QString, QList<TVNodeData> >();
      m_treeData.insert(nodeData->id, tvNodeData);

      parentCounts.insert(nodeData->id, 0);
    }

    int relationsCount = m_dataController->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataController->getRelationByIndex(i);
      TVNodeData sourceNodeData = m_treeData.value(relationData->sourceNodeId);
      TVNodeData destinationNodeData = m_treeData.value(relationData->destinationNodeId);

      QList<TVNodeData> children;
      if (sourceNodeData.childNodes->contains(relationData->name)) {
        children = sourceNodeData.childNodes->value(relationData->name);
      }
      children.append(destinationNodeData);
      sourceNodeData.childNodes->insert(relationData->name, children);
      parentCounts[relationData->destinationNodeId]++;
    }

    if (m_treeData.count() > 0) {
      QModelIndex nodeTreeIndex = m_objectsModel->index(0, 0);
      QList<TVNodeData> nodes;
      QSet<long> seenNodeIds;

      QList<int> counts = parentCounts.values();
      qSort(counts);
      int minCount = counts.at(0);
      foreach (long key, parentCounts.keys()) {
        int parentCount = parentCounts.value(key);
        if(parentCount == minCount) {
          nodes.append(m_treeData.value(key));
        }
      }

      QStandardItem *rootItem = m_objectsModel->itemFromIndex(nodeTreeIndex);
      int rowCount = rootItem->rowCount();
      rootItem->removeRows(0, rowCount);
      buildNodesTree(rootItem, nodes, &seenNodeIds);
    }
  }
}

void OntologyTreeViewController::updateTreeDataTopToBottom() {

  m_treeData.clear();

  if (m_dataController != NULL && m_dataController != NULL) {
    QMap<long, int> childCounts;

    int nodesCount = m_dataController->nodeCount();
    for (int i = 0; i < nodesCount; ++i) {
      NodeData *nodeData = m_dataController->getNodeByIndex(i);
      TVNodeData tvNodeData;
      tvNodeData.nodeData = nodeData;
      // FIX: leak
      tvNodeData.childNodes = new QMap<QString, QList<TVNodeData> >();
      m_treeData.insert(nodeData->id, tvNodeData);

      childCounts.insert(nodeData->id, 0);
    }

    int relationsCount = m_dataController->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataController->getRelationByIndex(i);
      TVNodeData sourceNodeData = m_treeData.value(relationData->sourceNodeId);
      TVNodeData destinationNodeData = m_treeData.value(relationData->destinationNodeId);

      QList<TVNodeData> children;
      if (destinationNodeData.childNodes->contains(relationData->name)) {
        children = destinationNodeData.childNodes->value(relationData->name);
      }
      children.append(sourceNodeData);
      destinationNodeData.childNodes->insert(relationData->name, children);
      childCounts[relationData->sourceNodeId]++;
    }

    if (m_treeData.count() > 0) {
      QModelIndex nodeTreeIndex = m_objectsModel->index(0, 0);
      QList<TVNodeData> nodes;
      QSet<long> seenNodeIds;

      QList<int> counts = childCounts.values();
      qSort(counts);
      int minCount = counts.at(0);
      foreach (long key, childCounts.keys()) {
        int parentCount = childCounts.value(key);
        if(parentCount == minCount) {
          nodes.append(m_treeData.value(key));
        }
      }

      QStandardItem *rootItem = m_objectsModel->itemFromIndex(nodeTreeIndex);
      int rowCount = rootItem->rowCount();
      rootItem->removeRows(0, rowCount);
      buildNodesTree(rootItem, nodes, &seenNodeIds);
    }
  }
}

void OntologyTreeViewController::buildNodesTree(QStandardItem *rootItem, QList<TVNodeData> nodes, QSet<long> *seenNodeIds) {

  QList<BuildTreeQueueItem> queue;

  // put all top level nodes
  for (int i = 0; i < nodes.count(); ++i) {
    TVNodeData nodeData = nodes.at(i);
    QStandardItem *item = new QStandardItem(nodeData.nodeData->name/* + "[" + QString::number(nodeData.nodeData->id) + "]"*/);
    item->setData((qlonglong)nodeData.nodeData->id, Qt::UserRole);
    rootItem->appendRow(item);

    if (!seenNodeIds->contains(nodeData.nodeData->id)) {
      // put all relation elements as 2nd level if aleady does not seen
      foreach (QString key, nodeData.childNodes->keys()) {
        QStandardItem *relationItem = new QStandardItem(key);
        relationItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->appendRow(relationItem);        

        // put nodes by relations
        QList<TVNodeData> children = nodeData.childNodes->value(key);
        BuildTreeQueueItem queueItem;
        queueItem.item = relationItem;
        queueItem.nodes = children;
        queue.append(queueItem);
      }

      seenNodeIds->insert(nodeData.nodeData->id);
    }
  }

  foreach (BuildTreeQueueItem queueItem, queue) {
    buildNodesTree(queueItem.item, queueItem.nodes, seenNodeIds);
  }
}

void OntologyTreeViewController::updateData() {

  clearTreeView();

  if (m_dataController != NULL && m_dataController != NULL) {
    QModelIndex nodesIndex = m_objectsModel->index(1, 0);
    int currentNodeRowsCount = m_objectsModel->rowCount(nodesIndex);
    int nodesCount = m_dataController->nodeCount();

    int countDiff = currentNodeRowsCount - nodesCount;
    if (countDiff > 0) {
      m_objectsModel->removeRows(0, countDiff, nodesIndex);
    }
    else if (countDiff < 0) {
      m_objectsModel->insertRows(0, abs(countDiff), nodesIndex);
    }

    for (int i = 0; i < nodesCount; ++i) {
      QModelIndex index = m_objectsModel->index(i, 0, nodesIndex);
      NodeData *nodeData = m_dataController->getNodeByIndex(i);
      m_objectsModel->setData(index, nodeData->name/* + "[" + QString::number(nodeData->id) + "]"*/);
      m_objectsModel->setData(index, (qlonglong)nodeData->id, Qt::UserRole);
      m_objectsModel->itemFromIndex(index)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    QModelIndex relationsIndex = m_objectsModel->index(2, 0);
    int currentRelationRowsCount = m_objectsModel->rowCount(relationsIndex);
    int relationsCount = m_dataController->relationCount();

    countDiff = currentRelationRowsCount - relationsCount;
    if (countDiff > 0) {
      m_objectsModel->removeRows(0, countDiff, relationsIndex);
    }
    else if (countDiff < 0) {
      m_objectsModel->insertRows(0, abs(countDiff), relationsIndex);
    }

    for (int i = 0; i < relationsCount; ++i) {
      QModelIndex index = m_objectsModel->index(i, 0, relationsIndex);
      RelationData *relationData = m_dataController->getRelationByIndex(i);
      m_objectsModel->setData(index, relationData->name/* + "[" + QString::number(relationData->id) + "]"*/);
      m_objectsModel->setData(index, (qlonglong)relationData->id, Qt::UserRole);
      m_objectsModel->itemFromIndex(index)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
  }

//  updateTreeDataBottomToTop();
  updateTreeDataTopToBottom();
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
