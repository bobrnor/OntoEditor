#ifndef LOGICALINFERENCE_H
#define LOGICALINFERENCE_H

#include <QtGui>

#include "ontology/IOntologyDataSource.h"

typedef struct {
    long id;
    QString name;
    QList<RelationData *> relations;
} LINodeData;

class LogicalInference {
  private:
    IOntologyDataSource *m_dataSource;
    QMap<long, LINodeData *> m_nodes;

    void setupInnerState();
    LINodeData *findNode(const QString &name) const;

  public:
    LogicalInference(IOntologyDataSource *dataSource);

    QString inference(const QString &query) const;
};

#endif // LOGICALINFERENCE_H
