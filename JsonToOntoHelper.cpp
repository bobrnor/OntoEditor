#include "JsonToOntoHelper.h"

#include <QDebug>

#include "ontology/IOntologyDataSource.h"
#include "ontology/IOntologyDelegate.h"

JsonToOntoHelper::JsonToOntoHelper() {

  m_languageDataSource = NULL;
  m_languageDelegate = NULL;
  m_destinationDataSource = NULL;
  m_destinationDelegate = NULL;
}

void JsonToOntoHelper::setLanguageOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_languageDataSource = dataSource;
  m_languageDelegate = delegate;
}

void JsonToOntoHelper::setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_destinationDataSource = dataSource;
  m_destinationDelegate = delegate;
}

void JsonToOntoHelper::fillOntology(const Json::Value &json) {

  qDebug() << "Start filling ontology";
  process(json, NULL, NULL);
}

NodeData *JsonToOntoHelper::copyNode(NodeData *currentLanguageNode, NodeData *currentDestinationNode) {

  NodeData *sourceNode = currentLanguageNode;
  NodeData *prevSourceNode = NULL;
  if (currentDestinationNode != NULL) {
    prevSourceNode = m_languageDataSource->findNode(currentDestinationNode->name, sourceNode);
  }

  int newNodeId = m_destinationDelegate->nodeCreated();
  m_destinationDelegate->nodeNameChanged(newNodeId, sourceNode->name);

  if (prevSourceNode != NULL) {
    foreach (long relationId, sourceNode->relations) {
      RelationData *relation = m_languageDataSource->getRelationById(relationId);
      if (relation->sourceNodeId == prevSourceNode->id) {
        long newRelationId = m_destinationDelegate->relatoinCreated(currentDestinationNode->id, newNodeId);
        m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
        break;
      }
      else if (relation->destinationNodeId == prevSourceNode->id) {
        long newRelationId = m_destinationDelegate->relatoinCreated(newNodeId, currentDestinationNode->id);
        m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
        break;
      }
    }
  }

  QPointF sourceNodePosition = m_languageDelegate->nodePosition(sourceNode->id);
  m_destinationDelegate->setNodePosition(newNodeId, sourceNodePosition);

  return m_destinationDataSource->getNodeById(newNodeId);
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
    NodeData *nodeForMember = m_languageDataSource->findNode(qMemberName, currentLanguageNode);

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

  NodeData *nodeForValue = m_destinationDataSource->findNode(qString, currentDestinationNode);
  if (nodeForValue == NULL) {
    // create node
    long nodeId = m_destinationDelegate->nodeCreated();
    m_destinationDelegate->nodeNameChanged(nodeId, qString);

    // create relation
    long relationId = m_destinationDelegate->relatoinCreated(nodeId, currentDestinationNode->id);
    m_destinationDelegate->relationNameChanged(relationId, "is_instance");
  }
}
