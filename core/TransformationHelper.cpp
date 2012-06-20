#include "TransformationHelper.h"

#include "lib_json/json/value.h"

TransformationHelper::TransformationHelper() {

  m_sourceDataSource = NULL;
  m_sourceDelegate = NULL;

  m_destinationDataSource = NULL;
  m_destinationDelegate = NULL;
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

void TransformationHelper::transform() {

  QSet<long> passedInstanceNodes;

  int relationsCount = m_sourceDataSource->relationCount();
  for (int i = 0; i < relationsCount; ++i) {
    RelationData *relation = m_sourceDataSource->getRelationByIndex(i);
    if (relation->name == "is_instance") {
      NodeData *nodeWithInstances = m_sourceDataSource->getNodeById(relation->destinationNodeId);
      if (!passedInstanceNodes.contains(nodeWithInstances->id)) {
        passedInstanceNodes.insert(nodeWithInstances->id);

        NodeData *targetNode = transformationTargetNode(nodeWithInstances);
        if (targetNode != NULL) {
          QStringList targetNodePath = m_problemsDataSource->pathToNode(targetNode->id);
          NodeData *destinationNode = addPathToDestinationOntology(targetNodePath);

          foreach (long relationId, nodeWithInstances->relations) {
            RelationData *relation = m_sourceDataSource->getRelationById(relationId);
            if (relation->name == "is_instance") {
              NodeData *instanceNode = m_sourceDataSource->getNodeById(relation->sourceNodeId);

              long newNodeId = m_destinationDelegate->nodeCreated();
              m_destinationDelegate->nodeNameChanged(newNodeId, instanceNode->name);
              long newRelationId = m_destinationDelegate->relationCreated(newNodeId, destinationNode->id);
              m_destinationDelegate->relationNameChanged(newRelationId, "is_instance");
            }
          }
        }
      }
    }
  }
}

NodeData *TransformationHelper::transformationTargetNode(NodeData *sourceNode) {

  QStringList pathToSourceNode = m_sourceDataSource->pathToNode(sourceNode->id);

  qDebug() << pathToSourceNode;

  NodeData *sourceNodeInProblemsOntology = m_problemsDataSource->getNodeByPath(pathToSourceNode);
  foreach (long relationId, sourceNodeInProblemsOntology->relations) {
    RelationData *relation = m_problemsDataSource->getRelationById(relationId);
    if (relation->name == "transform") {
      NodeData *targetNode = m_problemsDataSource->getNodeById(relation->destinationNodeId);
      Q_ASSERT(targetNode);
      return targetNode;
    }
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
