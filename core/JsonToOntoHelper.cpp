#include "JsonToOntoHelper.h"

#include <QDebug>

#include "OntologyDataController.h"

JsonToOntoHelper::JsonToOntoHelper() {

  m_languageOntologyDataController = NULL;
  m_destinationOntologyDataController = NULL;
}

void JsonToOntoHelper::setLanguageOntology(OntologyDataController *dataController) {

  m_languageOntologyDataController = dataController;
}

void JsonToOntoHelper::setDestinationOntology(OntologyDataController *dataController) {

  m_destinationOntologyDataController = dataController;
}

void JsonToOntoHelper::fillOntology(const Json::Value &json) {

  qDebug() << "Start filling ontology";
  process(json, NULL, NULL);
}

NodeData *JsonToOntoHelper::copyNode(NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  NodeData *sourceNode = currentLanguageNode;
  NodeData *prevSourceNode = NULL;
  if (currentDestinationNode != NULL) {
    prevSourceNode = m_languageOntologyDataController->findNode(currentDestinationNode->name, sourceNode);
  }

  int newNodeId = m_destinationOntologyDataController->nodeCreated();
  m_destinationOntologyDataController->nodeNameChanged(newNodeId, sourceNode->name);

  if (prevSourceNode != NULL) {
    foreach (long relationId, sourceNode->relations) {
      RelationData *relation = m_languageOntologyDataController->getRelationById(relationId);
      if (relation->sourceNodeId == prevSourceNode->id) {
        long newRelationId = m_destinationOntologyDataController->relationCreated(currentDestinationNode->id, newNodeId);
        m_destinationOntologyDataController->relationNameChanged(newRelationId, relation->name);
        break;
      }
      else if (relation->destinationNodeId == prevSourceNode->id) {
        long newRelationId = m_destinationOntologyDataController->relationCreated(newNodeId, currentDestinationNode->id);
        m_destinationOntologyDataController->relationNameChanged(newRelationId, relation->name);
        break;
      }
    }
  }

  QPointF sourceNodePosition = m_languageOntologyDataController->nodePosition(sourceNode->id);
  m_destinationOntologyDataController->setNodePosition(newNodeId, sourceNodePosition);

  return m_destinationOntologyDataController->getNodeById(newNodeId);
}

void JsonToOntoHelper::process(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  if (jsonValue.isArray()) {
    processArrayValue(jsonValue, currentLanguageNode, currentDestinationNode);
  }
  else if (jsonValue.isObject()) {
    processJsonValue(jsonValue, currentLanguageNode, currentDestinationNode);
  }
  else {
    processValue(jsonValue, currentLanguageNode, currentDestinationNode);
  }
}

void JsonToOntoHelper::processArrayValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  int size = jsonValue.size();
  for (int i = 0; i < size; ++i) {
    Json::Value item = jsonValue[i];
    process(item, currentLanguageNode, currentDestinationNode);
  }
}

void JsonToOntoHelper::processJsonValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  std::vector<std::string> members = jsonValue.getMemberNames();
  int size = members.size();
  for (int i = 0; i < size; ++i) {
    std::string memberName = members.at(i);
    Json::Value memberValue = jsonValue[memberName];

    QString qMemberName = QString::fromStdString(memberName);
    qDebug() << "Process " << qMemberName << " as json value";
    NodeData *nodeForMember = m_languageOntologyDataController->findNode(qMemberName, currentLanguageNode);

    Q_ASSERT_X(nodeForMember != NULL,
               "Processing json value",
               QString("Node with name `%1` not found").arg(qMemberName).toStdString().c_str());

    NodeData *newCurrentDestinationNode = copyNode(nodeForMember, currentDestinationNode);

    process(memberValue, nodeForMember, newCurrentDestinationNode);
  }
}

void JsonToOntoHelper::processValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  Q_ASSERT(currentLanguageNode != NULL);
  Q_ASSERT(currentDestinationNode != NULL);

  std::string jsonString = jsonValue.asString();
  QString qString = QString::fromStdString(jsonString);

  qDebug() << "Create node: " << qString << "as instance of " << currentDestinationNode->name;

  NodeData *languageNode = m_languageOntologyDataController->findNode(qString, currentLanguageNode);

  NodeData *nodeForValue = m_destinationOntologyDataController->findNode(qString, currentDestinationNode);
  if (nodeForValue == NULL) {
    // create node
    long nodeId = m_destinationOntologyDataController->nodeCreated();
    m_destinationOntologyDataController->nodeNameChanged(nodeId, qString);

    if (languageNode != NULL) {
      QPointF sourceNodePosition = m_languageOntologyDataController->nodePosition(languageNode->id);
      m_destinationOntologyDataController->setNodePosition(nodeId, sourceNodePosition);
    }

    // create relation
    long relationId = m_destinationOntologyDataController->relationCreated(nodeId, currentDestinationNode->id);
    m_destinationOntologyDataController->relationNameChanged(relationId, "is_instance");
  }
}
