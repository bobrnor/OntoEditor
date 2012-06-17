#include "LogicalInference.h"

#include "lib_json/json/value.h"
#include "JsonToOntoHelper.h"
#include "OntoToJsonHelper.h"

LogicalInference::LogicalInference() {

  m_sourceDataSource = NULL;
  m_sourceDelegate = NULL;

  m_destinationDataSource = NULL;
  m_destinationDelegate = NULL;
}

void LogicalInference::addLanguage(const QString &name, IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_languageDataSources.insert(name, dataSource);
  m_languageDelegates.insert(name, delegate);
}

void LogicalInference::removeLanguage(const QString &name) {

  m_languageDataSources.remove(name);
  m_languageDelegates.remove(name);
}

QList<QString> LogicalInference::availableLanguages() const {

  return m_languageDelegates.keys();
}

void LogicalInference::setSourceOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_sourceDataSource = dataSource;
  m_sourceDelegate = delegate;
}

IOntologyDataSource *LogicalInference::sourceDataSource() const {

  return m_sourceDataSource;
}

IOntologyDelegate *LogicalInference::sourceDelegate() const {

  return m_sourceDelegate;
}

void LogicalInference::setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_destinationDataSource = dataSource;
  m_destinationDelegate = delegate;
}

IOntologyDataSource *LogicalInference::destinationDataSource() const {

  return m_destinationDataSource;
}

IOntologyDelegate *LogicalInference::destinationDelegate() const {

  return m_destinationDelegate;
}

QString LogicalInference::findCorrenspondingLanguage(const QString &term) const {

  foreach (QString languageName, m_languageDataSources.keys()) {
    IOntologyDataSource *languageDataSource = m_languageDataSources.value(languageName);
    NodeData *node = languageDataSource->findNode(term);
    if (node != NULL) {
      return languageName;
    }
  }
  return QString::null;
}

//void LogicalInference::setupInnerState() {

//  m_nodes.clear();

//  if (m_dataSource != NULL) {
//    int nodesCount = m_dataSource->nodeCount();
//    for (int i = 0; i < nodesCount; ++i) {
//      LINodeData *liNodeData = new LINodeData();
//      NodeData *nodeData = m_dataSource->getNodeByIndex(i);
//      liNodeData->id = nodeData->id;
//      liNodeData->name = nodeData->name;
//      m_nodes.insert(liNodeData->id, liNodeData);
//    }

//    int relationsCount = m_dataSource->relationCount();
//    for (int i = 0; i < relationsCount; ++i) {
//      RelationData *relationData = m_dataSource->getRelationByIndex(i);
//      LINodeData *sourceNodeData = m_nodes.value(relationData->sourceNodeId);
//      LINodeData *destinationNodeData = m_nodes.value(relationData->destinationNodeId);
//      sourceNodeData->relations.append(relationData);
//      destinationNodeData->relations.append(relationData);
//    }
//  }
//}

void LogicalInference::updateData() {

}

void LogicalInference::dataChangedSlot() {

  updateData();
}

Json::Value LogicalInference::process(const Json::Value &value) {

  JsonToOntoHelper jtoHelper;
  jtoHelper.setLanguageOntology(m_languageDataSources.value("java"), m_languageDelegates.value("java"));
  jtoHelper.setDestinationOntology(m_sourceDataSource, m_sourceDelegate);
  jtoHelper.fillOntology(value);
  emit dataChangedSignal();
  return value;
//  transform();
//  updateData();

//  OntoToJsonHelper otjHelper(m_dataSource);
//  Json::Value newJson = otjHelper.generateJson();

//  emit dataChangedSignal();
//  return newJson;
}

void LogicalInference::transform() {

//  if (m_dataSource != NULL) {
//    int relationsCount = m_dataSource->relationCount();
//    for (int i = 0; i < relationsCount; ++i) {
//      RelationData *relationData = m_dataSource->getRelationByIndex(i);
//      if (relationData->name == "transform") {
//        LINodeData *sourceNode = m_nodes.value(relationData->sourceNodeId);
//        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);

//        foreach (RelationData *sourceNodeRelation, sourceNode->relations) {
//          if (sourceNodeRelation->destinationNodeId == sourceNode->id && sourceNodeRelation->name == "is_instance") {
//            LINodeData *instanceNode = m_nodes.value(sourceNodeRelation->sourceNodeId);
//            long newNodeId = m_delegate->nodeCreated();
//            m_delegate->nodeNameChanged(newNodeId, instanceNode->name);
//            long newRelationId = m_delegate->relatoinCreated(newNodeId, destinationNode->id);
//            m_delegate->relationNameChanged(newRelationId, "is_instance");
//          }
//        }
//      }
//    }
//  }
}

Json::Value LogicalInference::generate() {

//  Json::Value json;

//  if (m_dataSource != NULL) {
//    LINodeData *destinationBaseNode = NULL;

//    int relationsCount = m_dataSource->relationCount();
//    for (int i = 0; i < relationsCount; ++i) {
//      RelationData *relationData = m_dataSource->getRelationByIndex(i);
//      if (relationData->name == "transform") {
//        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);
//        destinationBaseNode = baseNode(destinationNode);
//        break;
//      }
//    }

//    if (destinationBaseNode != NULL) {
//      qDebug() << "Base node: " << destinationBaseNode->name;
//    }
//    else {
//      qDebug() << "Base node: (NULL)";
//    }

//    for (int i = 0; i < relationsCount; ++i) {
//      RelationData *relationData = m_dataSource->getRelationByIndex(i);
//      if (relationData->name == "is_instance") {
//        LINodeData *sourceNode = m_nodes.value(relationData->sourceNodeId);
//        LINodeData *destinationNode = m_nodes.value(relationData->destinationNodeId);

//        if (destinationBaseNode == baseNode(sourceNode)) {
//          bool isArray = false;
//          foreach (RelationData *destinationRelation, destinationNode->relations) {
//            if (destinationRelation->name == "is_element") {
//              isArray = true;
//              LINodeData *elementBaseNode = m_nodes.value(destinationRelation->destinationNodeId);
//              Json::Value arrayValue = json.get(elementBaseNode->name.toStdString(), Json::Value(Json::nullValue));
//              if (arrayValue.isNull()) {
//                arrayValue = Json::Value(Json::arrayValue);
//              }
//              Json::Value elementJson;
//              elementJson[destinationNode->name.toStdString()] = Json::Value(sourceNode->name.toStdString());
//              arrayValue.append(elementJson);
//              json[elementBaseNode->name.toStdString()] = arrayValue;
//            }

//            if (!isArray) {
//              json[destinationNode->name.toStdString()] = Json::Value(sourceNode->name.toStdString());
//            }
//          }
//        }
//      }
//    }
//  }

//  return json;
}

LINodeData *LogicalInference::baseNode(LINodeData *node) {

//  foreach (RelationData *relation, node->relations) {
//    if (relation->sourceNodeId == node->id) {
//      LINodeData *topNode = m_nodes.value(relation->destinationNodeId);
//      return baseNode(topNode);
//    }
//  }

//  return node;
}
