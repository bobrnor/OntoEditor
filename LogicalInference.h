#ifndef LOGICALINFERENCE_H
#define LOGICALINFERENCE_H

#include <QtGui>

#include "lib_json/json/json.h"
#include "ontology/IOntologyDataSource.h"
#include "ontology/IOntologyDelegate.h"
#include "LINodeData.h"

class LogicalInference : public QObject {
    Q_OBJECT

  private:
    IOntologyDelegate *m_delegate;
    IOntologyDataSource *m_dataSource;
    QMap<long, LINodeData *> m_nodes;

    void setupInnerState();

    void transform();
    Json::Value generate();
    LINodeData *baseNode(LINodeData *node);

  public:
    LogicalInference(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);

    void updateData();
    Json::Value process(const Json::Value &value);

  signals:
    void dataChangedSignal();

  public slots:
    void dataChangedSlot();
};

#endif // LOGICALINFERENCE_H
