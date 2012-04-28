#ifndef ONTOLOGYWIDGETDELEGATE_H
#define ONTOLOGYWIDGETDELEGATE_H

#include <QString>

class IOntologyDelegate {

  public:
    virtual long nodeCreated() = 0;
    virtual long relatoinCreated(long sourceNodeId, long destinationNodeId) = 0;
    virtual void nodeNameChanged(long nodeId, const QString &name) = 0;
    virtual void relationNameChanged(long relationId, const QString &name) = 0;
    virtual void nodeRemoved(long nodeId) = 0;
    virtual void relationRemoved(long relationId) = 0;
};

#endif // ONTOLOGYWIDGETDELEGATE_H
