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

void LogicalInference::setProblemsOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate) {

  m_problemsDataSource = dataSource;
  m_problemsDelegate = delegate;
}

IOntologyDataSource *LogicalInference::problemsDataSource() const {

  return m_problemsDataSource;
}

IOntologyDelegate *LogicalInference::problemsDelegate() const {

  return m_problemsDelegate;
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

void LogicalInference::updateData() {

}

void LogicalInference::dataChangedSlot() {

  updateData();
}

Json::Value LogicalInference::process(const Json::Value &value) {

  QString term = QString::fromStdString(value.getMemberNames().at(0));
  QString language = findCorrenspondingLanguage(term);

  JsonToOntoHelper jtoHelper;
  jtoHelper.setLanguageOntology(m_languageDataSources.value(language), m_languageDelegates.value(language));
  jtoHelper.setDestinationOntology(m_sourceDataSource, m_sourceDelegate);
  jtoHelper.fillOntology(value);

  transform();

  OntoToJsonHelper otjHelper(m_destinationDataSource);
  Json::Value newJson = otjHelper.generateJson();

  emit dataChangedSignal();
  return newJson;
//  return value;
}

void LogicalInference::transform() {

  Q_ASSERT(m_problemsDataSource != NULL);
  Q_ASSERT(m_problemsDelegate != NULL);

  QSet<long> passedInstanceNodes;

  int relationsCount = m_sourceDataSource->relationCount();
  for (int i = 0; i < relationsCount; ++i) {
    RelationData *relation = m_sourceDataSource->getRelationByIndex(i);
    if (relation->name == "is_instance") {
      NodeData *nodeWithInstances = m_sourceDataSource->getNodeById(relation->destinationNodeId);
      if (!passedInstanceNodes.contains(nodeWithInstances->id)) {
        passedInstanceNodes.insert(nodeWithInstances->id);

        NodeData *targetNode = transformationTargetNode(nodeWithInstances);
        if (targetNode != NULL) {
          QStringList targetNodePath = m_problemsDataSource->pathToNode(targetNode->id);
          NodeData *destinationNode = addPathToDestinationOntology(targetNodePath);

          foreach (long relationId, nodeWithInstances->relations) {
            RelationData *relation = m_sourceDataSource->getRelationById(relationId);
            if (relation->name == "is_instance") {
              NodeData *instanceNode = m_sourceDataSource->getNodeById(relation->sourceNodeId);

              long newNodeId = m_destinationDelegate->nodeCreated();
              m_destinationDelegate->nodeNameChanged(newNodeId, instanceNode->name);
              long newRelationId = m_destinationDelegate->relationCreated(newNodeId, destinationNode->id);
              m_destinationDelegate->relationNameChanged(newRelationId, "is_instance");
            }
          }
        }
      }
    }
  }
}

NodeData *LogicalInference::transformationTargetNode(NodeData *sourceNode) {

  QStringList pathToSourceNode = m_sourceDataSource->pathToNode(sourceNode->id);

  qDebug() << pathToSourceNode;

  NodeData *sourceNodeInProblemsOntology = m_problemsDataSource->getNodeByPath(pathToSourceNode);
  foreach (long relationId, sourceNodeInProblemsOntology->relations) {
    RelationData *relation = m_problemsDataSource->getRelationById(relationId);
    if (relation->name == "transform") {
      NodeData *targetNode = m_problemsDataSource->getNodeById(relation->destinationNodeId);
      Q_ASSERT(targetNode);
      return targetNode;
    }
  }
  return NULL;
}

NodeData *LogicalInference::addPathToDestinationOntology(const QStringList &path) {

  NodeData *prevDestinationNode = NULL;
  NodeData *prevProblemsNode = NULL;
  foreach (QString nodeName, path) {
    NodeData *destinationNode = m_destinationDataSource->findNode(nodeName, prevDestinationNode);
    NodeData *problemsNode = m_problemsDataSource->findNode(nodeName, prevProblemsNode);

    Q_ASSERT(problemsNode != NULL);

    if (destinationNode == NULL) {
      QPointF position = m_problemsDelegate->nodePosition(problemsNode->id);
      long newNodeId = m_destinationDelegate->nodeCreated();
      m_destinationDelegate->nodeNameChanged(newNodeId, problemsNode->name);
      destinationNode = m_destinationDataSource->getNodeById(newNodeId);

      if (prevDestinationNode != NULL) {
        RelationData *relation = m_problemsDataSource->getRelationByNodes(problemsNode->id, prevProblemsNode->id);
        if (relation != NULL) {
          long newRelationId = m_destinationDelegate->relationCreated(destinationNode->id, prevDestinationNode->id);
          m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
        }
        else {
          relation = m_problemsDataSource->getRelationByNodes(prevProblemsNode->id, problemsNode->id);
          if (relation != NULL) {
            long newRelationId = m_destinationDelegate->relationCreated(prevDestinationNode->id, destinationNode->id);
            m_destinationDelegate->relationNameChanged(newRelationId, relation->name);
          }
        }
      }

      m_destinationDelegate->setNodePosition(newNodeId, position);
    }

    prevDestinationNode = destinationNode;
    prevProblemsNode = problemsNode;
  }

  return prevDestinationNode;
}
