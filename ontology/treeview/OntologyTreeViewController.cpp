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

  m_objectsModel = new QStandardItemModel(3, 1);
  m_objectsTreeView->setModel(m_objectsModel);
  m_dataSource = NULL;
  m_delegate = NULL;

  QModelIndex nodeTreeIndex = m_objectsModel->index(0, 0);
  m_objectsModel->insertColumn(0, nodeTreeIndex);
  m_objectsModel->setData(nodeTreeIndex, tr("Tree"));

  QModelIndex nodeIndex = m_objectsModel->index(1, 0);
  m_objectsModel->insertColumn(0, nodeIndex);
  m_objectsModel->setData(nodeIndex, tr("Nodes"));

  QModelIndex relationIndex = m_objectsModel->index(2, 0);
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

void OntologyTreeViewController::updateTreeDataBottomToTop() {

  m_treeData.clear();

  if (m_dataSource != NULL) {
    QMap<long, int> parentCounts;

    int nodesCount = m_dataSource->nodeCount();
    for (int i = 0; i < nodesCount; ++i) {
      NodeData *nodeData = m_dataSource->getNodeByIndex(i);
      TVNodeData tvNodeData;
      tvNodeData.nodeData = nodeData;
      // FIX: leak
      tvNodeData.childNodes = new QMap<QString, QList<TVNodeData> >();
      m_treeData.insert(nodeData->id, tvNodeData);

      parentCounts.insert(nodeData->id, 0);
    }

    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
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

  if (m_dataSource != NULL) {
    QMap<long, int> childCounts;

    int nodesCount = m_dataSource->nodeCount();
    for (int i = 0; i < nodesCount; ++i) {
      NodeData *nodeData = m_dataSource->getNodeByIndex(i);
      TVNodeData tvNodeData;
      tvNodeData.nodeData = nodeData;
      // FIX: leak
      tvNodeData.childNodes = new QMap<QString, QList<TVNodeData> >();
      m_treeData.insert(nodeData->id, tvNodeData);

      childCounts.insert(nodeData->id, 0);
    }

    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
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
    QStandardItem *item = new QStandardItem(nodeData.nodeData->name + "[" + QString::number(nodeData.nodeData->id) + "]");
    item->setData((qlonglong)nodeData.nodeData->id, Qt::UserRole);
    rootItem->appendRow(item);

    if (!seenNodeIds->contains(nodeData.nodeData->id)) {
      // put all relation elements as 2nd level if aleady does not seen
      foreach (QString key, nodeData.childNodes->keys()) {
        QStandardItem *relationItem = new QStandardItem(key);
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

  if (m_dataSource != NULL) {
    QModelIndex nodesIndex = m_objectsModel->index(1, 0);
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
      NodeData *nodeData = m_dataSource->getNodeByIndex(i);
      m_objectsModel->setData(index, nodeData->name + "[" + QString::number(nodeData->id) + "]");
      m_objectsModel->setData(index, (qlonglong)nodeData->id, Qt::UserRole);
    }

    QModelIndex relationsIndex = m_objectsModel->index(2, 0);
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
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
      m_objectsModel->setData(index, relationData->name + "[" + QString::number(relationData->id) + "]");
      m_objectsModel->setData(index, (qlonglong)relationData->id, Qt::UserRole);
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
