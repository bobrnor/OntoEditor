#ifndef ONTOLOGYWIDGETDELEGATE_H
#define ONTOLOGYWIDGETDELEGATE_H

#include <QString>
#include <QPointF>

class IOntologyDelegate {

  public:
    virtual long nodeCreated() = 0;
    virtual long relationCreated(long sourceNodeId, long destinationNodeId) = 0;
    virtual void nodeNameChanged(long nodeId, const QString &name) = 0;
    virtual void relationNameChanged(long relationId, const QString &name) = 0;
    virtual void nodeRemoved(long nodeId) = 0;
    virtual void relationRemoved(long relationId) = 0;

    virtual QPointF nodePosition(long nodeId) const = 0;
    virtual void setNodePosition(long nodeId, const QPointF &position) = 0;
};

#endif // ONTOLOGYWIDGETDELEGATE_H
