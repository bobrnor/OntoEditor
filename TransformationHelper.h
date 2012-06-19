#ifndef LOGICALINFERENCE_H
#define LOGICALINFERENCE_H

#include <QtGui>

#include "lib_json/json/json.h"
#include "ontology/IOntologyDataSource.h"
#include "ontology/IOntologyDelegate.h"
#include "LINodeData.h"

class TransformationHelper : public QObject {
    Q_OBJECT

  private:
    IOntologyDataSource *m_sourceDataSource;
    IOntologyDelegate *m_sourceDelegate;

    IOntologyDataSource *m_destinationDataSource;
    IOntologyDelegate *m_destinationDelegate;

    IOntologyDataSource *m_problemsDataSource;
    IOntologyDelegate *m_problemsDelegate;

    void transform();
    NodeData *transformationTargetNode(NodeData *sourceNode);
    NodeData *addPathToDestinationOntology(const QStringList &path);

  public:
    TransformationHelper();

    void updateData();
    void process();

    void setSourceOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *sourceDataSource() const;
    IOntologyDelegate *sourceDelegate() const;

    void setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *destinationDataSource() const;
    IOntologyDelegate *destinationDelegate() const;

    void setProblemsOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *problemsDataSource() const;
    IOntologyDelegate *problemsDelegate() const;

    bool isReady() const;

  signals:
    void dataChangedSignal();

  public slots:
    void dataChangedSlot();
};

#endif // LOGICALINFERENCE_H
