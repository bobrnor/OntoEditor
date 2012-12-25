#ifndef LOGICALINFERENCE_H
#define LOGICALINFERENCE_H

#include <QtGui>

#include "lib_json/json/json.h"
#include "core/OntologyDataController.h"

class TransformationHelper : public QObject {
    Q_OBJECT

  private:
    OntologyDataController *m_sourceDataController;
    OntologyDataController *m_destinationDataController;
    OntologyDataController *m_problemsDataController;

    QStandardItemModel *m_logModel;
    QStandardItem *m_currentItem;

    void transform();
    NodeData *transformationTargetNode(NodeData *sourceNode);
    NodeData *addPathToDestinationOntology(const QStringList &path);

    void simpleTransform(NodeData *sourceNodeData, NodeData *destinationNodeData);
    void typeTransform(NodeData *sourceNodeData, NodeData *destinationNodeData);

    RelationData *transformRelation(NodeData *sourceNodeData);

  public:
    TransformationHelper();

    void updateData();
    void process();

    void setSourceOntology(OntologyDataController *dataController);
    OntologyDataController *sourceDataController() const;

    void setDestinationOntology(OntologyDataController *dataController);
    OntologyDataController *destinationDataController() const;

    void setProblemsOntology(OntologyDataController *dataController);
    OntologyDataController *problemsDataController() const;

    QStandardItemModel *logModel() const;

    bool isReady() const;

  signals:
    void dataChangedSignal();

  public slots:
    void dataChangedSlot();
};

#endif // LOGICALINFERENCE_H
