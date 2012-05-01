#ifndef LOGICALINFERENCE_H
#define LOGICALINFERENCE_H

#include <QtGui>

#include "ontology/IOntologyDataSource.h"

typedef struct {
    long id;
    QString name;
    QList<RelationData *> relations;
} LINodeData;

class LogicalInference : public QObject {
    Q_OBJECT

  private:
    IOntologyDataSource *m_dataSource;
    QMap<long, LINodeData *> m_nodes;

    void setupInnerState();
    LINodeData *findNode(const QString &name) const;

  public:
    LogicalInference(IOntologyDataSource *dataSource);

    void updateData();
    QString inference(const QString &query) const;

  public slots:
    void dataChangedSlot();
};

#endif // LOGICALINFERENCE_H
