#include "OntologyDataController.h"

OntologyDataController::OntologyDataController() {

  m_lastId = 0;
}

OntologyDataController::OntologyDataController(const Json::Value &json) {

  m_lastId = json["last_id"].asInt64();

  Json::Value itemsJson = json["items"];

  for (int i = 0; i < itemsJson.size(); ++i) {
    Json::Value itemJson = itemsJson[i];
    long id = itemJson["id"].asInt64();
    QString name = QString::fromStdString(itemJson["name"].asString());
    m_ontologyItems.insert(id, name);
  }
}

Json::Value OntologyDataController::serialize() {

  Json::Value value;
  value["last_id"] = Json::Value((Json::Int64)m_lastId);

  Json::Value itemsJson = Json::Value(Json::arrayValue);
  foreach (long key, m_ontologyItems.keys()) {
    QString name = m_ontologyItems[key];

    Json::Value itemJson;
    itemJson["id"] = Json::Value((Json::Int64)key);
    itemJson["name"] = Json::Value(name.toStdString());
    itemsJson.append(itemJson);
  }

  value["items"] = itemsJson;

  return value;
}

long OntologyDataController::nodeCreated() {

  m_lastId++;
  m_ontologyItems.insert(m_lastId, "");
  return m_lastId;
}

long OntologyDataController::relatoinCreated(long sourceNodeId, long destinationNodeId) {

  m_lastId++;
  m_ontologyItems.insert(m_lastId, "");
  return m_lastId;
}

void OntologyDataController::nodeNameChanged(long nodeId, const QString &name) {

  m_ontologyItems.insert(nodeId, name);
}

void OntologyDataController::relationNameChanged(long relationId, const QString &name) {

  m_ontologyItems.insert(relationId, name);
}

void OntologyDataController::nodeRemoved(long nodeId) {

  m_ontologyItems.remove(nodeId);
}

void OntologyDataController::relatoinRemoved(long relatoinId) {

  m_ontologyItems.remove(relatoinId);
}
