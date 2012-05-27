#include "JsonToOntoHelper.h"

#include <QDebug>

#include "ontology/IOntologyDataSource.h"
#include "ontology/IOntologyDelegate.h"

JsonToOntoHelper::JsonToOntoHelper(IOntologyDelegate *delegate, IOntologyDataSource *dataSource) {

  m_ontoDelegate = delegate;
  m_ontoDataSource = dataSource;
}

void JsonToOntoHelper::fillOntology(const Json::Value &json) {

  qDebug() << "Start filling ontology";
  process(json, NULL);
}

void JsonToOntoHelper::process(const Json::Value &jsonValue, NodeData *currentNode) {

  if (jsonValue.isArray()) {
    processArrayValue(jsonValue, currentNode);
  }
  else if (jsonValue.isObject()) {
    processJsonValue(jsonValue, currentNode);
  }
  else {
    processValue(jsonValue, currentNode);
  }
}

void JsonToOntoHelper::processArrayValue(const Json::Value &jsonValue, NodeData *currentNode) {

  int size = jsonValue.size();
  for (int i = 0; i < size; ++i) {
    Json::Value item = jsonValue[i];
    process(item, currentNode);
  }
}

void JsonToOntoHelper::processJsonValue(const Json::Value &jsonValue, NodeData *currentNode) {

  std::vector<std::string> members = jsonValue.getMemberNames();
  int size = members.size();
  for (int i = 0; i < size; ++i) {
    std::string memberName = members.at(i);
    Json::Value memberValue = jsonValue[memberName];

    QString qMemberName = QString::fromStdString(memberName);
    qDebug() << "Process " << qMemberName << " as json value";
    NodeData *nodeForMember = m_ontoDataSource->findNode(qMemberName, currentNode);

    Q_ASSERT_X(nodeForMember != NULL,
               "Processing json value",
               QString("Node with name `%1` not found").arg(qMemberName).toStdString().c_str());

    process(memberValue, nodeForMember);
  }
}

void JsonToOntoHelper::processValue(const Json::Value &jsonValue, NodeData *currentNode) {

  Q_ASSERT(currentNode != NULL);

  std::string jsonString = jsonValue.asString();
  QString qString = QString::fromStdString(jsonString);

  qDebug() << "Create node: " << qString << "as instance of " << currentNode->name;

  NodeData *nodeForValue = m_ontoDataSource->findNode(qString, currentNode);
  if (nodeForValue == NULL) {
    // create node
    long nodeId = m_ontoDelegate->nodeCreated();
    m_ontoDelegate->nodeNameChanged(nodeId, qString);

    // create relation
    long relationId = m_ontoDelegate->relatoinCreated(nodeId, currentNode->id);
    m_ontoDelegate->relationNameChanged(relationId, "is_instance");
  }
}
