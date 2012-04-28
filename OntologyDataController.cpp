#include "OntologyDataController.h"

OntologyDataController::OntologyDataController() {

  m_lastId = 0;
}

OntologyDataController::OntologyDataController(const Json::Value &json) {

  m_lastId = json["last_id"].asInt64();

  Json::Value nodesJson = json["nodes"];
  for (int i = 0; i < nodesJson.size(); ++i) {
    Json::Value itemJson = nodesJson[i];
    NodeData *nodeData = new NodeData();
    nodeData->id = itemJson["id"].asInt64();
    nodeData->name = QString::fromStdString(itemJson["name"].asString());
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
  }
}

Json::Value OntologyDataController::serialize() {

  Json::Value value;
  value["last_id"] = Json::Value((Json::Int64)m_lastId);

  Json::Value nodesJson = Json::Value(Json::arrayValue);
  foreach (NodeData *nodeData, m_nodesList) {
    Json::Value itemJson;
    itemJson["id"] = Json::Value((Json::Int64)nodeData->id);
    itemJson["name"] = Json::Value(nodeData->name.toStdString());
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

// data source

int OntologyDataController::nodeCount() {

  return m_nodesList.count();
}

int OntologyDataController::relationCount() {

  return m_relationsList.count();
}

NodeData *OntologyDataController::node(int index) {

  return m_nodesList.at(index);
}

RelationData *OntologyDataController::relation(int index) {

  return m_relationsList.at(index);
}

// delegate

long OntologyDataController::nodeCreated() {

  m_lastId++;

  NodeData *node = new NodeData();
  node->id = m_lastId;
  node->name = "";

  m_nodesMap.insert(node->id, node);
  m_nodesList.append(node);

  return node->id;
}

long OntologyDataController::relatoinCreated(long sourceNodeId, long destinationNodeId) {

  m_lastId++;

  RelationData *relation = new RelationData();
  relation->id = m_lastId;
  relation->name = "";
  relation->sourceNodeId = sourceNodeId;
  relation->destinationNodeId = destinationNodeId;

  m_relationsMap.insert(relation->id, relation);
  m_relationsList.append(relation);

  return relation->id;
}

void OntologyDataController::nodeNameChanged(long nodeId, const QString &name) {

  NodeData *node = m_nodesMap.value(nodeId);
  node->name = name;
}

void OntologyDataController::relationNameChanged(long relationId, const QString &name) {

  RelationData *relation = m_relationsMap.value(relationId);
  relation->name = name;
}

void OntologyDataController::nodeRemoved(long nodeId) {

  NodeData *node = m_nodesMap.value(nodeId);
  removeRelatedRelations(node);
  m_nodesMap.remove(nodeId);
  m_nodesList.removeAll(node);
  delete node;
}

void OntologyDataController::relationRemoved(long relationId) {

  RelationData *relation = m_relationsMap.value(relationId);
  m_relationsMap.remove(relationId);
  m_relationsList.removeAll(relation);
  delete relation;
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
    delete relationData;
  }
}