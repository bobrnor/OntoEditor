#include "LogicalInference.h"

#include "lib_json/json/value.h"

LogicalInference::LogicalInference(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_delegate = delegate;
  m_dataSource = dataSource;
  setupInnerState();
}

void LogicalInference::setupInnerState() {

  m_nodes.clear();

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
    if (QString::compare(nodeData->name, name, Qt::CaseInsensitive) == 0) {
      return nodeData;
    }
  }
  return NULL;
}

LINodeData *LogicalInference::findNode(const QString &name, LINodeData *nearestNode) const {

  foreach (RelationData *relationData, nearestNode->relations) {
    LINodeData *sourceNode = m_nodes.value(relationData->sourceNodeId);
    LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);
    LINodeData *testNode = (sourceNode == nearestNode) ? destinationNode : sourceNode;

    if (QString::compare(testNode->name, name, Qt::CaseInsensitive) == 0) {
      return testNode;
    }
  }

  return NULL;
}

void LogicalInference::updateData() {

  setupInnerState();
}

void LogicalInference::dataChangedSlot() {

  updateData();
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

      LINodeData *node = m_nodes.value(30);
      foreach (RelationData *relation, node->relations) {
        if (relation->sourceNodeId == node->id) {
          LINodeData *destNode = m_nodes.value(relation->destinationNodeId);
          if (destNode->relations.count() == 2) {
            return destNode->name;
          }
        }
      }
    }
  }
  return QString::null;
}

Json::Value LogicalInference::process(const Json::Value &value) {

  std::vector<std::string> valueMembers = value.getMemberNames();
  for (int i = 0; i < valueMembers.size(); ++i) {
    qDebug() << "i: " << i;
    QString nodeName = QString::fromStdString(valueMembers.at(i));

    qDebug() << "Find node name: " << nodeName;

    LINodeData *nodeData = findNode(nodeName);
    if (nodeData != NULL) {
      Json::Value memberValue = value[valueMembers.at(i)];
      if (memberValue.isArray()) {
        qDebug() << "Element is array with size: " << memberValue.size();

        for (int j = 0; j < memberValue.size(); ++j) {
          qDebug() << "j: " << j;

          Json::Value arrayElementJson = memberValue[j];
          std::vector<std::string> arrayElementMembers = arrayElementJson.getMemberNames();
          for (int k = 0; k < arrayElementMembers.size(); ++k) {
            qDebug() << "k: " << k;
            QString arrayElementMemberName = QString::fromStdString(arrayElementMembers.at(k));

            qDebug() << "Find node name: " << arrayElementMemberName;

            LINodeData *elementNode = findNode(arrayElementMemberName, nodeData);
            if (elementNode != NULL) {
              QString nodeInstanceName = QString::fromStdString(arrayElementJson[arrayElementMembers.at(k)].asString());
              LINodeData *instanceNode = findNode(nodeInstanceName, elementNode);
              if (instanceNode == NULL) {
                qDebug() << "Create node with name: " << nodeInstanceName;

                long newNodeId = m_delegate->nodeCreated();
                m_delegate->nodeNameChanged(newNodeId, nodeInstanceName);
                long newRelatoinId = m_delegate->relatoinCreated(newNodeId, elementNode->id);
                m_delegate->relationNameChanged(newRelatoinId, "is_instance");
              }
            }
          }
        }
      }
      else {
        QString instanceName = QString::fromStdString(value[valueMembers.at(i)].asString());
        LINodeData *instanceNode = findNode(instanceName, nodeData);
        if (instanceNode == NULL) {
          qDebug() << "Create node with name: " << instanceName;

          long newNodeId = m_delegate->nodeCreated();
          m_delegate->nodeNameChanged(newNodeId, instanceName);
          long newRelationId = m_delegate->relatoinCreated(newNodeId, nodeData->id);
          m_delegate->relationNameChanged(newRelationId, "is_instance");
        }
      }
    }
  }

  updateData();
  transform();
  updateData();
  Json::Value json = generate();

  emit dataChangedSignal();

  return json;
}

void LogicalInference::transform() {

  if (m_dataSource != NULL) {
    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->relation(i);
      if (relationData->name == "transform") {
        LINodeData *sourceNode = m_nodes.value(relationData->sourceNodeId);
        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);

        foreach (RelationData *sourceNodeRelation, sourceNode->relations) {
          if (sourceNodeRelation->destinationNodeId == sourceNode->id && sourceNodeRelation->name == "is_instance") {
            LINodeData *instanceNode = m_nodes.value(sourceNodeRelation->sourceNodeId);
            long newNodeId = m_delegate->nodeCreated();
            m_delegate->nodeNameChanged(newNodeId, instanceNode->name);
            long newRelationId = m_delegate->relatoinCreated(newNodeId, destinationNode->id);
            m_delegate->relationNameChanged(newRelationId, "is_instance");
          }
        }
      }
    }
  }
}

Json::Value LogicalInference::generate() {

  Json::Value json;

  if (m_dataSource != NULL) {
    LINodeData *destinationBaseNode = NULL;

    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->relation(i);
      if (relationData->name == "transform") {
        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);
        destinationBaseNode = baseNode(destinationNode);
        break;
      }
    }

    if (destinationBaseNode != NULL) {
      qDebug() << "Base node: " << destinationBaseNode->name;
    }
    else {
      qDebug() << "Base node: (NULL)";
    }

    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->relation(i);
      if (relationData->name == "is_instance") {
        LINodeData *sourceNode = m_nodes.value(relationData->sourceNodeId);
        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);

        if (destinationBaseNode == baseNode(sourceNode)) {
          bool isArray = false;
          foreach (RelationData *destinationRelation, destinationNode->relations) {
            if (destinationRelation->name == "is_element") {
              isArray = true;
              LINodeData *elementBaseNode = m_nodes.value(destinationRelation->destinationNodeId);
              Json::Value arrayValue = json.get(elementBaseNode->name.toStdString(), Json::Value(Json::nullValue));
              if (arrayValue.isNull()) {
                arrayValue = Json::Value(Json::arrayValue);
              }
              Json::Value elementJson;
              elementJson[destinationNode->name.toStdString()] = Json::Value(sourceNode->name.toStdString());
              arrayValue.append(elementJson);
              json[elementBaseNode->name.toStdString()] = arrayValue;
            }

            if (!isArray) {
              json[destinationNode->name.toStdString()] = Json::Value(sourceNode->name.toStdString());
            }
          }
        }
      }
    }
  }

  return json;
}

LINodeData *LogicalInference::baseNode(LINodeData *node) {

  foreach (RelationData *relation, node->relations) {
    if (relation->sourceNodeId == node->id) {
      LINodeData *topNode = m_nodes.value(relation->destinationNodeId);
      return baseNode(topNode);
    }
  }

  return node;
}
