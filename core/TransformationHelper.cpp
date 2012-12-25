#include "TransformationHelper.h"

#include "lib_json/json/value.h"

TransformationHelper::TransformationHelper() {

  m_sourceDataSource = NULL;
  m_sourceDelegate = NULL;

  m_destinationDataSource = NULL;
  m_destinationDelegate = NULL;

  m_logModel = new QStandardItemModel();
  m_currentItem = NULL;
}

void TransformationHelper::setSourceOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_sourceDataSource = dataSource;
  m_sourceDelegate = delegate;
}

IOntologyDataSource *TransformationHelper::sourceDataSource() const {

  return m_sourceDataSource;
}

IOntologyDelegate *TransformationHelper::sourceDelegate() const {

  return m_sourceDelegate;
}

void TransformationHelper::setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_destinationDataSource = dataSource;
  m_destinationDelegate = delegate;
}

IOntologyDataSource *TransformationHelper::destinationDataSource() const {

  return m_destinationDataSource;
}

IOntologyDelegate *TransformationHelper::destinationDelegate() const {

  return m_destinationDelegate;
}

void TransformationHelper::setProblemsOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_problemsDataSource = dataSource;
  m_problemsDelegate = delegate;
}

IOntologyDataSource *TransformationHelper::problemsDataSource() const {

  return m_problemsDataSource;
}

IOntologyDelegate *TransformationHelper::problemsDelegate() const {

  return m_problemsDelegate;
}

bool TransformationHelper::isReady() const {

  if (m_sourceDataSource != NULL && m_sourceDelegate != NULL
      && m_destinationDataSource != NULL && m_destinationDelegate != NULL
      && m_problemsDataSource != NULL && m_problemsDelegate != NULL) {
    return true;
  }
  else {
    return false;
  }
}

void TransformationHelper::updateData() {

}

void TransformationHelper::dataChangedSlot() {

  updateData();
}

void TransformationHelper::process() {

  if (isReady()) {
    transform();
    emit dataChangedSignal();
  }
}

QStandardItemModel *TransformationHelper::logModel() const {

  return m_logModel;
}

RelationData *TransformationHelper::transformRelation(NodeData *sourceNodeData) {

  QStringList pathToSourceNode = m_sourceDataSource->pathToNode(sourceNodeData->id);
  NodeData *sourceNodeInProblemsOntology = m_problemsDataSource->getNodeByPath(pathToSourceNode);

  foreach (long relationId, sourceNodeInProblemsOntology->relations) {
    RelationData *relation = m_problemsDataSource->getRelationById(relationId);
    if (relation->name == "transform" || relation->name == "type_transform") {
      return relation;
    }
  }

  return NULL;
}

void TransformationHelper::transform() {

  m_logModel->clear();
  m_logModel->insertColumns(0, 1);
  m_logModel->insertRows(0, 1);

  QModelIndex rootIndex = m_logModel->index(0, 0);
  m_logModel->insertColumn(0, rootIndex);
  m_logModel->setData(rootIndex, tr("Log"));
  m_logModel->itemFromIndex(rootIndex)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  m_currentItem = NULL;

  QSet<long> passedInstanceNodes;

  int relationsCount = m_sourceDataSource->relationCount();
  for (int i = 0; i < relationsCount; ++i) {
    RelationData *relation = m_sourceDataSource->getRelationByIndex(i);
    if (relation->name == "is_instance") {

      QStandardItem *searchForItem = new QStandardItem(tr("Search for instance..."));
      m_currentItem = m_logModel->itemFromIndex(rootIndex);
      m_currentItem->appendRow(searchForItem);

      NodeData *nodeWithInstances = m_sourceDataSource->getNodeById(relation->destinationNodeId);

      QStandardItem *foundItem = new QStandardItem(nodeWithInstances->name + tr(" found"));
      searchForItem->appendRow(foundItem);

      if (!passedInstanceNodes.contains(nodeWithInstances->id)) {
        passedInstanceNodes.insert(nodeWithInstances->id);

        QStandardItem *searchTransItem = new QStandardItem(tr("Search in problem ontology"));
        foundItem->appendRow(searchTransItem);
        m_currentItem = foundItem;

        NodeData *targetNode = transformationTargetNode(nodeWithInstances);
        if (targetNode != NULL) {

          QStandardItem *foundTransItem = new QStandardItem(tr("Transformation target node found: ") + targetNode->name);
          m_currentItem->appendRow(foundTransItem);

          QStringList targetNodePath = m_problemsDataSource->pathToNode(targetNode->id);
          NodeData *destinationNode = addPathToDestinationOntology(targetNodePath);

          QStandardItem *addPathItem = new QStandardItem(tr("Add transformation target node to destination ontology"));
          foundTransItem->appendRow(addPathItem);

          m_currentItem = addPathItem;

          RelationData *transformRelationData = transformRelation(nodeWithInstances);
          if (transformRelationData != NULL) {
            if (transformRelationData->name == "transform") {
              simpleTransform(nodeWithInstances, destinationNode);
            }
            else if (transformRelationData->name == "type_transform") {
              typeTransform(nodeWithInstances, destinationNode);
            }
          }
        }
        else {
          QStandardItem *item = new QStandardItem(tr("Not found"));
          foundItem->appendRow(item);
        }
      }
      else {
        QStandardItem *item = new QStandardItem(nodeWithInstances->name + tr(" already processed"));
        foundItem->appendRow(item);
      }
    }
  }
}

/*
  @sourceNodeData - node from source ontology,
  @destinationNodeData - node from destination ontology
*/
void TransformationHelper::simpleTransform(NodeData *sourceNodeData, NodeData *destinationNodeData) {

  QStandardItem *transItem = new QStandardItem(tr("Transformation..."));
  m_currentItem->appendRow(transItem);

  foreach (long relationId, sourceNodeData->relations) {
    RelationData *relation = m_sourceDataSource->getRelationById(relationId);
    if (relation->name == "is_instance") {
      NodeData *instanceNode = m_sourceDataSource->getNodeById(relation->sourceNodeId);

      QStandardItem *getItem = new QStandardItem(tr("GET ") + instanceNode->name);
      transItem->appendRow(getItem);

      QStandardItem *fromItem = new QStandardItem(tr("FROM ") + sourceNodeData->name);
      transItem->appendRow(fromItem);

      QStandardItem *toItem = new QStandardItem(tr("PUT TO ") + destinationNodeData->name);
      transItem->appendRow(toItem);

      long newNodeId = m_destinationDelegate->nodeCreated();
      m_destinationDelegate->nodeNameChanged(newNodeId, instanceNode->name);
      long newRelationId = m_destinationDelegate->relationCreated(newNodeId, destinationNodeData->id);
      m_destinationDelegate->relationNameChanged(newRelationId, "is_instance");
    }
  }
}

/*
  @sourceNodeData - node from source ontology,
  @destinationNodeData - node from destination ontology
*/
void TransformationHelper::typeTransform(NodeData *sourceNodeData, NodeData *destinationNodeData) {

  QStandardItem *transItem = new QStandardItem(tr("Transformation..."));
  m_currentItem->appendRow(transItem);

  QStringList destinationNodePath = m_destinationDataSource->pathToNode(destinationNodeData->id);
  NodeData *problemsDestinationNode = m_problemsDataSource->getNodeByPath(destinationNodePath);

  foreach (long relationId, problemsDestinationNode->relations) {
    RelationData *relationData = m_problemsDataSource->getRelationById(relationId);

    if (relationData->name == "is_instance") {
      NodeData *typeNodeData = m_problemsDataSource->getNodeById(relationData->sourceNodeId);
      Q_ASSERT(typeNodeData);

      QStandardItem *getItem = new QStandardItem(tr("GET ") + sourceNodeData->name);
      transItem->appendRow(getItem);

      QStandardItem *toItem = new QStandardItem(tr("PUT TO ") + destinationNodeData->name);
      transItem->appendRow(toItem);

      QStandardItem *asItem = new QStandardItem(tr("AS ") + typeNodeData->name);
      transItem->appendRow(asItem);

      long newNodeId = m_destinationDelegate->nodeCreated();
      m_destinationDelegate->nodeNameChanged(newNodeId, typeNodeData->name);
      long newRelationId = m_destinationDelegate->relationCreated(newNodeId, sourceNodeData->id);
      m_destinationDelegate->relationNameChanged(newRelationId, "is_instance");
    }
  }
}

NodeData *TransformationHelper::transformationTargetNode(NodeData *sourceNode) {

  RelationData *transformRelationData = transformRelation(sourceNode);
  if (transformRelationData != NULL) {

    QStandardItem *foundTransItem = new QStandardItem(tr("Node in problem ontology found"));
    m_currentItem->appendRow(foundTransItem);
    m_currentItem = foundTransItem;

    QStandardItem *searchTargetTransItem = new QStandardItem(tr("Search for transformation target node..."));
    m_currentItem->appendRow(searchTargetTransItem);
    m_currentItem = searchTargetTransItem;

    NodeData *targetNode = m_problemsDataSource->getNodeById(transformRelationData->destinationNodeId);
    Q_ASSERT(targetNode);
    return targetNode;
  }

  return NULL;
}

NodeData *TransformationHelper::addPathToDestinationOntology(const QStringList &path) {

  NodeData *prevDestinationNode = NULL;
  NodeData *prevProblemsNode = NULL;
  foreach (QString nodeName, path) {
    NodeData *destinationNode = m_destinationDataSource->findNode(nodeName, prevDestinationNode);
    NodeData *problemsNode = m_problemsDataSource->findNode(nodeName, prevProblemsNode);

    Q_ASSERT(problemsNode != NULL);

    if (destinationNode == NULL) {
      QPointF position = m_problemsDelegate->nodePosition(problemsNode->id);
      long newNodeId = m_destinationDelegate->nodeCreated();
      m_destinationDelegate->nodeNameChanged(newNodeId, problemsNode->name);
      destinationNode = m_destinationDataSource->getNodeById(newNodeId);

      if (prevDestinationNode != NULL) {
        RelationData *relation = m_problemsDataSource->getRelationByNodes(problemsNode->id, prevProblemsNode->id);
        if (relation != NULL) {
          long newRelationId = m_destinationDelegate->relationCreated(destinationNode->id, prevDestinationNode->id);
          m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
        }
        else {
          relation = m_problemsDataSource->getRelationByNodes(prevProblemsNode->id, problemsNode->id);
          if (relation != NULL) {
            long newRelationId = m_destinationDelegate->relationCreated(prevDestinationNode->id, destinationNode->id);
            m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
          }
        }
      }

      m_destinationDelegate->setNodePosition(newNodeId, position);
    }

    prevDestinationNode = destinationNode;
    prevProblemsNode = problemsNode;
  }

  return prevDestinationNode;
}
