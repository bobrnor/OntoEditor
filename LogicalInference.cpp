#include "LogicalInference.h"

#include "lib_json/json/value.h"
#include "JsonToOntoHelper.h"
#include "OntoToJsonHelper.h"

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
      NodeData *nodeData = m_dataSource->getNodeByIndex(i);
      liNodeData->id = nodeData->id;
      liNodeData->name = nodeData->name;
      m_nodes.insert(liNodeData->id, liNodeData);
    }

    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
      LINodeData *sourceNodeData = m_nodes.value(relationData->sourceNodeId);
      LINodeData *destinationNodeData = m_nodes.value(relationData->destinationNodeId);
      sourceNodeData->relations.append(relationData);
      destinationNodeData->relations.append(relationData);
    }
  }
}

void LogicalInference::updateData() {

  setupInnerState();
}

void LogicalInference::dataChangedSlot() {

  updateData();
}

Json::Value LogicalInference::process(const Json::Value &value) {

  JsonToOntoHelper jtoHelper(m_delegate, m_dataSource);
  jtoHelper.fillOntology(value);
  updateData();
  transform();
  updateData();

  OntoToJsonHelper otjHelper(m_dataSource);
  Json::Value newJson = otjHelper.generateJson();

  emit dataChangedSignal();
  return value;
}

void LogicalInference::transform() {

  if (m_dataSource != NULL) {
    int relationsCount = m_dataSource->relationCount();
    for (int i = 0; i < relationsCount; ++i) {
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
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
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
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
      RelationData *relationData = m_dataSource->getRelationByIndex(i);
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
