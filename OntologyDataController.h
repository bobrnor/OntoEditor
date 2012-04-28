#ifndef ONTOLOGYDATACONTROLLER_H
#define ONTOLOGYDATACONTROLLER_H

#include <QtGui>

#include "lib_json/json/json.h"

#include "ontology/IOntologyDataSource.h"
#include "ontology/IOntologyDelegate.h"

class OntologyDataController : public IOntologyDataSource, public IOntologyDelegate {
  private:
    long m_lastId;
    // mapping
    QMap<long, NodeData *> m_nodesMap;
    QMap<long, RelationData *> m_relationsMap;
    // listing
    QList<NodeData *> m_nodesList;
    QList<RelationData *> m_relationsList;

    void removeRelatedRelations(NodeData *nodeData);

  public:
    OntologyDataController();
    OntologyDataController(const Json::Value &json);

    // data source
    int nodeCount();
    int relationCount();
    NodeData *node(int index);
    RelationData *relation(int index);

    // delegate
    long nodeCreated();
    long relatoinCreated(long sourceNodeId, long destinationNodeId);
    void nodeNameChanged(long nodeId, const QString &name);
    void relationNameChanged(long relationId, const QString &name);
    void nodeRemoved(long nodeId);
    void relationRemoved(long relationId);

    Json::Value serialize();
};

#endif // ONTOLOGYDATACONTROLLER_H
