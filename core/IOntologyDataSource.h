#ifndef IONTOLOGYDATASOURCE_H
#define IONTOLOGYDATASOURCE_H

#include "NodeData.h"
#include "RelationData.h"

class IOntologyDataSource {
  public:
    virtual int nodeCount() = 0;
    virtual int relationCount() = 0;

    virtual NodeData *getNodeByIndex(int index) = 0;
    virtual RelationData *getRelationByIndex(int index) = 0;

    virtual NodeData *getNodeById(long id) = 0;
    virtual RelationData *getRelationById(long id) = 0;

    virtual NodeData *getNodeByPath(const QStringList &path) const = 0;

    virtual RelationData *getRelationByNodes(long sourceNodeId, long destinationNodeId) = 0;

    virtual NodeData *findNode(const QString &nodeName) const = 0;
    virtual NodeData *findNode(const QString &nodeName, NodeData *startNode) const = 0;

    virtual QStringList pathToNode(long id) = 0;
};

#endif // IONTOLOGYDATASOURCE_H
