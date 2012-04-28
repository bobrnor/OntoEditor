#ifndef IONTOLOGYDATASOURCE_H
#define IONTOLOGYDATASOURCE_H

#include "OntologyGraphElement.h"

typedef struct {
    long id;
    QString name;
} NodeData;

typedef struct {
    long id;
    long sourceNodeId;
    long destinationNodeId;
    QString name;
} RelationData;

class IOntologyDataSource {
  public:
    virtual int nodeCount() = 0;
    virtual int relationCount() = 0;
    virtual NodeData *node(int index) = 0;
    virtual RelationData *relation(int index) = 0;
};

#endif // IONTOLOGYDATASOURCE_H
