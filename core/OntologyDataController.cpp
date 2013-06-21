#include "OntologyDataController.h"

OntologyDataController::OntologyDataController() {

  m_lastId = 0;

  m_changedNodeIds = QSet<long>();
  m_changedRelationIds = QSet<long>();
}

OntologyDataController::OntologyDataController(const Json::Value &json) {

  deserialize(json);

  m_changedNodeIds = QSet<long>();
  m_changedRelationIds = QSet<long>();
}

OntologyDataController::OntologyDataController(QList<NodeData *> nodeList,
                                               QList<RelationData *> relationList,
                                               QMap<long, QPointF> nodePositions,
                                               QSet<long> changedNodeIds,
                                               QSet<long> changedRelationIds) {

  m_lastId = -1;

  foreach (NodeData *nodeData, nodeList) {
    m_nodesMap.insert(nodeData->id, nodeData);
    m_nodesList.append(nodeData);
  }

  foreach (RelationData *relationData, relationList) {
    m_relationsMap.insert(relationData->id, relationData);
    m_relationsList.append(relationData);
    m_relationsMapByNodes.insert(QPair<long, long>(relationData->sourceNodeId, relationData->destinationNodeId), relationData);
  }

  m_nodePositions = nodePositions;
  m_changedNodeIds = changedNodeIds;
  m_changedRelationIds = changedRelationIds;

  qDebug() << "Snapshot created: " << m_changedNodeIds << "\n" << m_changedRelationIds;
}

void OntologyDataController::setSourceCode(const QString &sourceCode) {

  m_sourceCode = sourceCode;
}

void OntologyDataController::normalize() {

  double minX = INFINITY;
  double minY = INFINITY;

  foreach (long nodeId, m_nodePositions.keys()) {
    QPointF position = m_nodePositions.value(nodeId);
    minX = qMin(position.x(), minX);
    minY = qMin(position.y(), minY);
  }

  minX -= 200;
  minY -= 200;

  foreach (long nodeId, m_nodePositions.keys()) {
    QPointF position = m_nodePositions.value(nodeId);
    position.setX(position.x() - minX);
    position.setY(position.y() - minY);
    m_nodePositions.insert(nodeId, position);
  }
}

Json::Value OntologyDataController::serialize() const {

  Json::Value value;
  value["last_id"] = Json::Value((Json::Int64)m_lastId);

  Json::Value nodesJson = Json::Value(Json::arrayValue);
  foreach (NodeData *nodeData, m_nodesList) {
    Json::Value itemJson;
    itemJson["id"] = Json::Value((Json::Int64)nodeData->id);
    itemJson["name"] = Json::Value(nodeData->name.toStdString());
    itemJson["position_x"] = Json::Value(nodePosition(nodeData->id).x());
    itemJson["position_y"] = Json::Value(nodePosition(nodeData->id).y());
    nodesJson.append(itemJson);
  }
  value["nodes"] = nodesJson;

  Json::Value relationsJson = Json::Value(Json::arrayValue);
  foreach (RelationData *relationData, m_relationsList) {
    Json::Value itemJson;
    itemJson["id"] = Json::Value((Json::Int64)relationData->id);
    itemJson["name"] = Json::Value(relationData->name.toStdString());
    itemJson["source_node_id"] = Json::Value((Json::Int64)relationData->sourceNodeId);
    itemJson["destination_node_id"] = Json::Value((Json::Int64)relationData->destinationNodeId);
    relationsJson.append(itemJson);
  }
  value["relations"] = relationsJson;

  return value;
}

void OntologyDataController::deserialize(const Json::Value &json) {

  qDebug() << QString::fromStdString(json.toStyledString());

  m_lastId = json["last_id"].asInt64();

  Json::Value nodesJson = json["nodes"];
  for (int i = 0; i < nodesJson.size(); ++i) {
    Json::Value itemJson = nodesJson[i];
    NodeData *nodeData = new NodeData();
    nodeData->id = itemJson["id"].asInt64();
    nodeData->name = QString::fromStdString(itemJson["name"].asString());
    if (itemJson.getMemberNames().size() == 4) {
      double x = itemJson["position_x"].asDouble();
      double y = itemJson["position_y"].asDouble();
      setNodePosition(nodeData->id, QPointF(x, y));
    }
    m_nodesMap.insert(nodeData->id, nodeData);
    m_nodesList.append(nodeData);
  }

  Json::Value relationsJson = json["relations"];
  for (int i = 0; i < relationsJson.size(); ++i) {
    Json::Value itemJson = relationsJson[i];
    RelationData *relationData = new RelationData();
    relationData->id = itemJson["id"].asInt64();
    relationData->name = QString::fromStdString(itemJson["name"].asString());
    relationData->sourceNodeId = itemJson["source_node_id"].asInt64();
    relationData->destinationNodeId = itemJson["destination_node_id"].asInt64();
    m_relationsMap.insert(relationData->id, relationData);
    m_relationsList.append(relationData);

    QPair<long, long> relationNodesPair(relationData->sourceNodeId, relationData->destinationNodeId);
    m_relationsMapByNodes.insert(relationNodesPair, relationData);

    NodeData *sourceNode = m_nodesMap.value(relationData->sourceNodeId);
    sourceNode->relations.append(relationData->id);

    NodeData *destinationNode = m_nodesMap.value(relationData->destinationNodeId);
    destinationNode->relations.append(relationData->id);
  }

  normalize();
}

// data source

int OntologyDataController::nodeCount() {

  return m_nodesList.count();
}

int OntologyDataController::relationCount() {

  return m_relationsList.count();
}

NodeData *OntologyDataController::getNodeByIndex(int index, bool asChanges) {

  NodeData *nodeData = m_nodesList.at(index);
  if (asChanges) {
    m_changedNodeIds.insert(nodeData->id);
  }
  return nodeData;
}

RelationData *OntologyDataController::getRelationByIndex(int index, bool asChanges) {

  RelationData *relationData = m_relationsList.at(index);
  if (asChanges) {
    m_changedRelationIds.insert(relationData->id);
  }
  return relationData;
}

NodeData *OntologyDataController::getNodeById(long id, bool asChanges) {

  if (asChanges) {
    m_changedNodeIds.insert(id);
  }
  return m_nodesMap.value(id);
}

RelationData *OntologyDataController::getRelationById(long id, bool asChanges) {

  if (asChanges) {
    m_changedRelationIds.insert(id);
  }
  return m_relationsMap.value(id);
}

NodeData *OntologyDataController::getNodeByPath(const QStringList &path) const {

  NodeData *prevNode = NULL;
  foreach (QString nodeName, path) {
    NodeData *node = findNode(nodeName, prevNode);
    Q_ASSERT(node != NULL);
    prevNode = node;
  }
  m_changedNodeIds.insert(prevNode->id);
  return prevNode;
}

RelationData *OntologyDataController::getRelationByNodes(long sourceNodeId, long destinationNodeId) {

  QPair<long, long> nodesPair(sourceNodeId, destinationNodeId);
  RelationData *relationData = m_relationsMapByNodes.value(nodesPair);
  m_changedRelationIds.insert(relationData->id);
  return relationData;
}

NodeData *OntologyDataController::findNode(const QString &nodeName) const {

  foreach (NodeData *nodeData, m_nodesList) {
    if (nodeData->name.compare(nodeName, Qt::CaseInsensitive) == 0) {
      m_changedNodeIds.insert(nodeData->id);
      return nodeData;
    }
  }
  return NULL;
}

NodeData *OntologyDataController::findNode(const QString &nodeName, NodeData *startNode) const {

  if (startNode == NULL) {
    return findNode(nodeName);
  }

  foreach (long relationId, startNode->relations) {
    RelationData *relation = m_relationsMap.value(relationId);
    if (relation->name.compare("transform", Qt::CaseInsensitive) != 0
        && relation->name.compare("type_transform", Qt::CaseInsensitive) != 0) {
      NodeData *node = otherNode(relation, startNode);
      if (node->name.compare(nodeName, Qt::CaseInsensitive) == 0) {
        m_changedNodeIds.insert(node->id);
        return node;
      }
    }
  }

  qDebug() << QString("Can not find node `%1` from node `%2`").arg(nodeName, startNode->name);

  return NULL;
}

QStringList OntologyDataController::pathToNode(long id) {

  m_changedNodeIds.insert(id);

  QStringList path;
  NodeData *currentNode = NULL;
  NodeData *nextNode = getNodeById(id);

  while (nextNode != NULL) {
    currentNode = nextNode;
    nextNode = NULL;
    path.push_front(currentNode->name);

    foreach (long relationId, currentNode->relations) {
      RelationData *relation = getRelationById(relationId);
      if (relation->sourceNodeId == currentNode->id) {
        nextNode = getNodeById(relation->destinationNodeId);
        break;
      }
    }
  }

  return path;
}

NodeData *OntologyDataController::otherNode(RelationData *relation, NodeData *node) const {

  if (relation->sourceNodeId == node->id) {
    return m_nodesMap.value(relation->destinationNodeId);
  }
  else {
    return m_nodesMap.value(relation->sourceNodeId);
  }
}

QString OntologyDataController::sourceCode() const {

  return m_sourceCode;
}

// delegate

long OntologyDataController::nodeCreated() {

  m_lastId++;

  NodeData *node = new NodeData();
  node->id = m_lastId;
  node->name = "";

  m_nodesMap.insert(node->id, node);
  m_nodesList.append(node);

  m_changedNodeIds.insert(node->id);

  return node->id;
}

long OntologyDataController::relationCreated(long sourceNodeId, long destinationNodeId) {

  m_lastId++;

  RelationData *relation = new RelationData();
  relation->id = m_lastId;
  relation->name = "";
  relation->sourceNodeId = sourceNodeId;
  relation->destinationNodeId = destinationNodeId;

  m_relationsMap.insert(relation->id, relation);
  m_relationsList.append(relation);

  QPair<long, long> relationNodesPair(relation->sourceNodeId, relation->destinationNodeId);
  m_relationsMapByNodes.insert(relationNodesPair, relation);

  NodeData *sourceNode = m_nodesMap.value(relation->sourceNodeId);
  sourceNode->relations.append(relation->id);

  NodeData *destinationNode = m_nodesMap.value(relation->destinationNodeId);
  destinationNode->relations.append(relation->id);

  m_changedRelationIds.insert(relation->id);

  return relation->id;
}

void OntologyDataController::nodeNameChanged(long nodeId, const QString &name) {

  NodeData *node = m_nodesMap.value(nodeId);
  node->name = name;

  m_changedNodeIds.insert(node->id);
}

void OntologyDataController::relationNameChanged(long relationId, const QString &name) {

  RelationData *relation = m_relationsMap.value(relationId);
  relation->name = name;

  m_changedRelationIds.insert(relation->id);
}

void OntologyDataController::nodeAttributesChanged(long nodeId, const QMap<QString, QString> &attributes) {

  NodeData *node = m_nodesMap.value(nodeId);
  node->attributes = attributes;

  m_changedNodeIds.insert(node->id);
}

void OntologyDataController::relationAttributesChanged(long relationId, const QMap<QString, QString> &attributes) {

  RelationData *relation = m_relationsMap.value(relationId);
  relation->attributes = attributes;

  m_changedRelationIds.insert(relation->id);
}

void OntologyDataController::nodeRemoved(long nodeId) {

  NodeData *node = m_nodesMap.value(nodeId);
  removeRelatedRelations(node);
  m_nodesMap.remove(nodeId);
  m_nodesList.removeAll(node);
  delete node;
}

void OntologyDataController::relationRemoved(long relationId) {

  qDebug() << m_relationsMap;

  RelationData *relation = m_relationsMap.value(relationId);

  if (relation != NULL) {
    m_relationsMap.remove(relationId);
    m_relationsList.removeAll(relation);

    QPair<long, long> relationNodesPair(relation->sourceNodeId, relation->destinationNodeId);
    m_relationsMapByNodes.remove(relationNodesPair);

    delete relation;
  }
}

void OntologyDataController::removeRelatedRelations(NodeData *nodeData) {

  QList<RelationData *> relationsToRemove;
  foreach (RelationData *relationData, m_relationsList) {
    if (relationData->sourceNodeId == nodeData->id || relationData->destinationNodeId == nodeData->id) {
      relationsToRemove.append(relationData);
    }
  }

  foreach (RelationData *relationData, relationsToRemove) {
    m_relationsList.removeAll(relationData);
    m_relationsMap.remove(relationData->id);

    QPair<long, long> relationNodesPair(relationData->sourceNodeId, relationData->destinationNodeId);
    m_relationsMapByNodes.remove(relationNodesPair);

    delete relationData;
  }
}

QPointF OntologyDataController::nodePosition(long nodeId) const {

  if (m_nodePositions.contains(nodeId)) {
    return m_nodePositions.value(nodeId);
  }
  else {
    return QPointF(0.0, 0.0);
  }
}

void OntologyDataController::setNodePosition(long nodeId, const QPointF &position) {

  qDebug() << "Position changed!";
  m_nodePositions.insert(nodeId, position);
}

bool OntologyDataController::isNodeChanged(long id) const {

  return m_changedNodeIds.contains(id);
}

bool OntologyDataController::isRelationChanged(long id) const {

  return m_changedRelationIds.contains(id);
}

bool OntologyDataController::hasChanges() const {

  return m_changedNodeIds.count() > 0 || m_changedRelationIds.count() > 0;
}

int OntologyDataController::changesCount() const {

  return m_changedNodeIds.count() + m_changedRelationIds.count();
}

void OntologyDataController::clearChanges() {

  m_changedNodeIds.clear();
  m_changedRelationIds.clear();
}

OntologyDataController *OntologyDataController::makeSnapshot() {

  OntologyDataController *snapshot = new OntologyDataController(m_nodesList,
                                                                m_relationsList,
                                                                m_nodePositions,
                                                                m_changedNodeIds,
                                                                m_changedRelationIds);

  return snapshot;
}
