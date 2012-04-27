#include "OntologyDataController.h"

OntologyDataController::OntologyDataController() {

  m_lastId = 0;
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
