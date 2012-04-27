#ifndef ONTOLOGYDATACONTROLLER_H
#define ONTOLOGYDATACONTROLLER_H

#include <QMap>
#include <QString>

#include "IOntologyDataSource.h"
#include "IOntologyWidgetDelegate.h"

class OntologyDataController : public IOntologyDataSource, public IOntologyWidgetDelegate {
  private:
    long m_lastId;
    QMap<long, QString> m_ontologyItems;

  public:
    OntologyDataController();

    long nodeCreated();
    long relatoinCreated(long sourceNodeId, long destinationNodeId);
    void nodeNameChanged(long nodeId, const QString &name);
    void relationNameChanged(long relationId, const QString &name);
    void nodeRemoved(long nodeId);
    void relatoinRemoved(long relatoinId);
};

#endif // ONTOLOGYDATACONTROLLER_H
