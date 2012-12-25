#include "TransformationHelper.h"

#include "lib_json/json/value.h"

TransformationHelper::TransformationHelper() {

  m_sourceDataController = NULL;
  m_destinationDataController = NULL;
  m_problemsDataController = NULL;

  m_logModel = new QStandardItemModel();
  m_currentItem = NULL;
}

void TransformationHelper::setSourceOntology(OntologyDataController *dataController) {

  m_sourceDataController = dataController;
}

OntologyDataController *TransformationHelper::sourceDataController() const {

  return m_sourceDataController;
}

void TransformationHelper::setDestinationOntology(OntologyDataController *dataController) {

  m_destinationDataController = dataController;
}

OntologyDataController *TransformationHelper::destinationDataController() const {

  return m_destinationDataController;
}

void TransformationHelper::setProblemsOntology(OntologyDataController *dataController) {

  m_problemsDataController = dataController;
}

OntologyDataController *TransformationHelper::problemsDataController() const {

  return m_problemsDataController;
}

bool TransformationHelper::isReady() const {

  if (m_sourceDataController != NULL && m_destinationDataController != NULL && m_problemsDataController != NULL) {
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

  QStringList pathToSourceNode = m_sourceDataController->pathToNode(sourceNodeData->id);
  NodeData *sourceNodeInProblemsOntology = m_problemsDataController->getNodeByPath(pathToSourceNode);

  foreach (long relationId, sourceNodeInProblemsOntology->relations) {
    RelationData *relation = m_problemsDataController->getRelationById(relationId);
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

  int relationsCount = m_sourceDataController->relationCount();
  for (int i = 0; i < relationsCount; ++i) {
    RelationData *relation = m_sourceDataController->getRelationByIndex(i);
    if (relation->name == "is_instance") {

      QStandardItem *searchForItem = new QStandardItem(tr("Search for instance..."));
      m_currentItem = m_logModel->itemFromIndex(rootIndex);
      m_currentItem->appendRow(searchForItem);

      NodeData *nodeWithInstances = m_sourceDataController->getNodeById(relation->destinationNodeId);

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

          QStringList targetNodePath = m_problemsDataController->pathToNode(targetNode->id);
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
    RelationData *relation = m_sourceDataController->getRelationById(relationId);
    if (relation->name == "is_instance") {
      NodeData *instanceNode = m_sourceDataController->getNodeById(relation->sourceNodeId);

      QStandardItem *getItem = new QStandardItem(tr("GET ") + instanceNode->name);
      transItem->appendRow(getItem);

      QStandardItem *fromItem = new QStandardItem(tr("FROM ") + sourceNodeData->name);
      transItem->appendRow(fromItem);

      QStandardItem *toItem = new QStandardItem(tr("PUT TO ") + destinationNodeData->name);
      transItem->appendRow(toItem);

      long newNodeId = m_destinationDataController->nodeCreated();
      m_destinationDataController->nodeNameChanged(newNodeId, instanceNode->name);
      long newRelationId = m_destinationDataController->relationCreated(newNodeId, destinationNodeData->id);
      m_destinationDataController->relationNameChanged(newRelationId, "is_instance");
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

  QStringList destinationNodePath = m_destinationDataController->pathToNode(destinationNodeData->id);
  NodeData *problemsDestinationNode = m_problemsDataController->getNodeByPath(destinationNodePath);

  foreach (long relationId, problemsDestinationNode->relations) {
    RelationData *relationData = m_problemsDataController->getRelationById(relationId);

    if (relationData->name == "is_instance") {
      NodeData *typeNodeData = m_problemsDataController->getNodeById(relationData->sourceNodeId);
      Q_ASSERT(typeNodeData);

      QStandardItem *getItem = new QStandardItem(tr("GET ") + sourceNodeData->name);
      transItem->appendRow(getItem);

      QStandardItem *toItem = new QStandardItem(tr("PUT TO ") + destinationNodeData->name);
      transItem->appendRow(toItem);

      QStandardItem *asItem = new QStandardItem(tr("AS ") + typeNodeData->name);
      transItem->appendRow(asItem);

      long newNodeId = m_destinationDataController->nodeCreated();
      m_destinationDataController->nodeNameChanged(newNodeId, typeNodeData->name);
      long newRelationId = m_destinationDataController->relationCreated(newNodeId, sourceNodeData->id);
      m_destinationDataController->relationNameChanged(newRelationId, "is_instance");
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

    NodeData *targetNode = m_problemsDataController->getNodeById(transformRelationData->destinationNodeId);
    Q_ASSERT(targetNode);
    return targetNode;
  }

  return NULL;
}

NodeData *TransformationHelper::addPathToDestinationOntology(const QStringList &path) {

  NodeData *prevDestinationNode = NULL;
  NodeData *prevProblemsNode = NULL;
  foreach (QString nodeName, path) {
    NodeData *destinationNode = m_destinationDataController->findNode(nodeName, prevDestinationNode);
    NodeData *problemsNode = m_problemsDataController->findNode(nodeName, prevProblemsNode);

    Q_ASSERT(problemsNode != NULL);

    if (destinationNode == NULL) {
      QPointF position = m_problemsDataController->nodePosition(problemsNode->id);
      long newNodeId = m_destinationDataController->nodeCreated();
      m_destinationDataController->nodeNameChanged(newNodeId, problemsNode->name);
      destinationNode = m_destinationDataController->getNodeById(newNodeId);

      if (prevDestinationNode != NULL) {
        RelationData *relation = m_problemsDataController->getRelationByNodes(problemsNode->id, prevProblemsNode->id);
        if (relation != NULL) {
          long newRelationId = m_destinationDataController->relationCreated(destinationNode->id, prevDestinationNode->id);
          m_destinationDataController->relationNameChanged(newRelationId, relation->name);
        }
        else {
          relation = m_problemsDataController->getRelationByNodes(prevProblemsNode->id, problemsNode->id);
          if (relation != NULL) {
            long newRelationId = m_destinationDataController->relationCreated(prevDestinationNode->id, destinationNode->id);
            m_destinationDataController->relationNameChanged(newRelationId, relation->name);
          }
        }
      }

      m_destinationDataController->setNodePosition(newNodeId, position);
    }

    prevDestinationNode = destinationNode;
    prevProblemsNode = problemsNode;
  }

  return prevDestinationNode;
}
