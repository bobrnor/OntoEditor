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
    QMap<QPair<long, long>, RelationData *> m_relationsMapByNodes;
    // listing
    QList<NodeData *> m_nodesList;
    QList<RelationData *> m_relationsList;

    void removeRelatedRelations(NodeData *nodeData);
    NodeData *otherNode(RelationData *relation, NodeData *node) const;

  public:
    OntologyDataController();
    OntologyDataController(const Json::Value &json);

    // data source
    int nodeCount();
    int relationCount();

    NodeData *nodeByIndex(int index);
    RelationData *relationByIndex(int index);

    NodeData *nodeById(long id);
    RelationData *relationById(long id);

    RelationData *relationByNodes(long sourceNodeId, long destinationNodeId);

    NodeData *findNode(const QString &nodeName) const;
    NodeData *findNode(const QString &nodeName, NodeData *startNode) const;

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
