#ifndef ONTOLOGYDATACONTROLLER_H
#define ONTOLOGYDATACONTROLLER_H

#include <QtGui>

#include "lib_json/json/json.h"

#include "core/IOntologyDataSource.h"
#include "core/IOntologyDelegate.h"

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

    QMap<long, QPointF> m_nodePositions;

    QString m_sourceCode;

    void removeRelatedRelations(NodeData *nodeData);
    NodeData *otherNode(RelationData *relation, NodeData *node) const;

  public:
    OntologyDataController();
    OntologyDataController(const Json::Value &json);

    void setSourceCode(const QString &sourceCode);

    void normalize();

    // data source
    int nodeCount();
    int relationCount();

    NodeData *getNodeByIndex(int index);
    RelationData *getRelationByIndex(int index);

    NodeData *getNodeById(long id);
    RelationData *getRelationById(long id);

    NodeData *getNodeByPath(const QStringList &path) const;

    RelationData *getRelationByNodes(long sourceNodeId, long destinationNodeId);

    NodeData *findNode(const QString &nodeName) const;
    NodeData *findNode(const QString &nodeName, NodeData *startNode) const;

    QStringList pathToNode(long id);

    QString sourceCode() const;

    // delegate
    long nodeCreated();
    long relationCreated(long sourceNodeId, long destinationNodeId);
    void nodeNameChanged(long nodeId, const QString &name);
    void relationNameChanged(long relationId, const QString &name);
    void nodeRemoved(long nodeId);
    void relationRemoved(long relationId);

    QPointF nodePosition(long nodeId) const;
    void setNodePosition(long nodeId, const QPointF &position);

    Json::Value serialize();
};

#endif // ONTOLOGYDATACONTROLLER_H
