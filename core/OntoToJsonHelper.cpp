#include "OntoToJsonHelper.h"

#include <QDebug>

#include "OntologyDataController.h"

OntoToJsonHelper::OntoToJsonHelper(OntologyDataController *dataController) {

  m_dataController = dataController;
}

Json::Value OntoToJsonHelper::generateJson() {

  NodeData *someNode = m_dataController->getNodeByIndex(0);
  NodeData *root = rootNode(someNode);
  if (root == NULL) {
    root = someNode;
  }

  Q_ASSERT_X(root != NULL, "Finding root node", "Can't find root node");

  Json::Value json;
  putNodeIntoJson(root, &json);
  return json;
}

void OntoToJsonHelper::putNodeIntoJson(NodeData *node, Json::Value *jsonValue) const {

  qDebug() << "Put " << node->name;

  Json::Value nodeJson;

  foreach (long relationId, node->relations) {
    RelationData *relation = m_dataController->getRelationById(relationId);
    if (relation->name.compare("transform", Qt::CaseInsensitive) != 0
        /*&& relation->name.compare("is_element", Qt::CaseInsensitive) != 0*/) {

      if (relation->destinationNodeId == node->id) {
        if (relation->name.compare("is_instance", Qt::CaseInsensitive) == 0) {
          qDebug() << "Put instances:";

          QList<NodeData *> instances = getAllInstances(node);

          if (instances.count() > 1) {
            nodeJson = Json::Value(Json::arrayValue);

            foreach (NodeData *instance, instances) {
              qDebug() << "Put value " << instance->name;
              nodeJson.append(Json::Value(instance->name.toStdString()));
            }
          }
          else {
            nodeJson = Json::Value(instances.at(0)->name.toStdString());
          }

          break;
        }
        else {
          NodeData *nextNode = m_dataController->getNodeById(relation->sourceNodeId);
          putNodeIntoJson(nextNode, &nodeJson);
        }
      }
    }
  }

  (*jsonValue)[node->name.toStdString()] = nodeJson;
}

QList<NodeData *> OntoToJsonHelper::getAllInstances(NodeData *node) const {

  QList<NodeData *> instances;

  foreach (long relationId, node->relations) {
    RelationData *relation = m_dataController->getRelationById(relationId);
    if (relation->destinationNodeId == node->id && relation->name.compare("is_instance") == 0) {
      NodeData *instanceNode = m_dataController->getNodeById(relation->sourceNodeId);
      instances.append(instanceNode);
    }
  }

  return instances;
}

NodeData *OntoToJsonHelper::rootNode(NodeData *node) const {

  foreach (long relationId, node->relations) {
    RelationData *relation = m_dataController->getRelationById(relationId);
    if (relation->sourceNodeId == node->id) {
      NodeData *parentNode = m_dataController->getNodeById(relation->destinationNodeId);
      NodeData *root = rootNode(parentNode);
      if (root == NULL) {
        return parentNode;
      }
      else {
        return root;
      }
    }
  }
  return NULL;
}
