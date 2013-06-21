#ifndef ONTOLOGYDATACONTROLLER_H
#define ONTOLOGYDATACONTROLLER_H

#include <QtGui>

#include "lib_json/json/json.h"

#include "NodeData.h"
#include "RelationData.h"

class OntologyDataController {
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

    mutable QSet<long> m_changedNodeIds;
    mutable QSet<long> m_changedRelationIds;

    QString m_sourceCode;

    void removeRelatedRelations(NodeData *nodeData);
    NodeData *otherNode(RelationData *relation, NodeData *node) const;

    OntologyDataController(QList<NodeData *> nodeList,
                           QList<RelationData *> relationList,
                           QMap<long, QPointF> nodePositions,
                           QSet<long> changedNodeIds,
                           QSet<long> changedRelationIds);

  public:
    OntologyDataController();
    OntologyDataController(const Json::Value &json);

    void setSourceCode(const QString &sourceCode);

    void normalize();

    OntologyDataController *makeSnapshot();

    // data source
    int nodeCount();
    int relationCount();

    NodeData *getNodeByIndex(int index, bool asChanges = false);
    RelationData *getRelationByIndex(int index, bool asChanges = false);

    NodeData *getNodeById(long id, bool asChanges = false);
    RelationData *getRelationById(long id, bool asChanges = false);

    NodeData *getNodeByPath(const QStringList &path) const;

    RelationData *getRelationByNodes(long sourceNodeId, long destinationNodeId);

    NodeData *findNode(const QString &nodeName) const;
    NodeData *findNode(const QString &nodeName, NodeData *startNode) const;

    QStringList pathToNode(long id);

    QString sourceCode() const;

    bool isNodeChanged(long id) const;
    bool isRelationChanged(long id) const;
    bool hasChanges() const;
    int changesCount() const;
    void clearChanges();

    // delegate
    long nodeCreated();
    long relationCreated(long sourceNodeId, long destinationNodeId);
    void nodeNameChanged(long nodeId, const QString &name);
    void relationNameChanged(long relationId, const QString &name);
    void nodeAttributesChanged(long nodeId, const QMap<QString, QString> &attributes);
    void relationAttributesChanged(long relationId, const QMap<QString, QString> &attributes);
    void nodeRemoved(long nodeId);
    void relationRemoved(long relationId);

    QPointF nodePosition(long nodeId) const;
    void setNodePosition(long nodeId, const QPointF &position);

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);
};

#endif // ONTOLOGYDATACONTROLLER_H
