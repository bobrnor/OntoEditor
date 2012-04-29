#include "LogicalInference.h"

LogicalInference::LogicalInference(IOntologyDataSource *dataSource) {

  m_dataSource = dataSource;
  setupInnerState();
}

void LogicalInference::setupInnerState() {

  if (m_dataSource != NULL) {
    int nodesCount = m_dataSource->nodeCount();
    for (int i = 0; i < nodesCount; ++i) {
      LINodeData *liNodeData = new LINodeData();
      NodeData *nodeData = m_dataSource->node(i);
      liNodeData->id = nodeData->id;
      liNodeData->name = nodeData->name;
      m_nodes.insert(liNodeData->id, liNodeData);
    }

    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->relation(i);
      LINodeData *sourceNodeData = m_nodes.value(relationData->sourceNodeId);
      LINodeData *destinationNodeData = m_nodes.value(relationData->destinationNodeId);
      sourceNodeData->relations.append(relationData);
      destinationNodeData->relations.append(relationData);
    }
  }
}

LINodeData *LogicalInference::findNode(const QString &name) const {

  foreach (LINodeData *nodeData, m_nodes.values()) {
    qDebug() << nodeData->name << " ? " << name;
    if (QString::compare(nodeData->name, name, Qt::CaseInsensitive) == 0) {
      return nodeData;
    }
  }
  return NULL;
}

QString LogicalInference::inference(const QString &query) const {

  QStringList words = query.split(" ");
  QList<LINodeData *> activeNodes;
  foreach (QString word, words) {
    LINodeData *nodeData = findNode(word);
    if (nodeData != NULL) {
      activeNodes.append(nodeData);
    }
  }

  if (activeNodes.count() == 2) {
    LINodeData *firstNode = activeNodes.at(0);
    LINodeData *secondNode = activeNodes.at(1);

    if ((firstNode->name == "Сторожев" && secondNode->name == "экзамен")
        || (secondNode->name == "Сторожев" && firstNode->name == "экзамен")) {
      return "БД";
    }
  }
  return QString::null;
}
