#ifndef IONTOLOGYDATASOURCE_H
#define IONTOLOGYDATASOURCE_H

#include "NodeData.h"
#include "RelationData.h"

class IOntologyDataSource {
  public:
    virtual int nodeCount() = 0;
    virtual int relationCount() = 0;

    virtual NodeData *nodeByIndex(int index) = 0;
    virtual RelationData *relationByIndex(int index) = 0;

    virtual NodeData *nodeById(long id) = 0;
    virtual RelationData *relationById(long id) = 0;

    virtual RelationData *relationByNodes(long sourceNodeId, long destinationNodeId) = 0;

    virtual NodeData *findNode(const QString &nodeName) const = 0;
    virtual NodeData *findNode(const QString &nodeName, NodeData *startNode) const = 0;
};

#endif // IONTOLOGYDATASOURCE_H
