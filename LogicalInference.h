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
    QMap<QString, IOntologyDataSource *> m_languageDataSources;
    QMap<QString, IOntologyDelegate *> m_languageDelegates;

    IOntologyDataSource *m_sourceDataSource;
    IOntologyDelegate *m_sourceDelegate;

    IOntologyDataSource *m_destinationDataSource;
    IOntologyDelegate *m_destinationDelegate;

    IOntologyDataSource *m_problemsDataSource;
    IOntologyDelegate *m_problemsDelegate;

//    QMap<long, LINodeData *> m_nodes;

    QString findCorrenspondingLanguage(const QString &term) const;
//    void setupInnerState();

    void transform();
    NodeData *transformationTargetNode(NodeData *sourceNode);
    NodeData *addPathToDestinationOntology(const QStringList &path);

  public:
    LogicalInference();

    void updateData();
    Json::Value process(const Json::Value &value);

    void addLanguage(const QString &name, IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    void removeLanguage(const QString &name);
    QList<QString> availableLanguages() const;

    void setSourceOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *sourceDataSource() const;
    IOntologyDelegate *sourceDelegate() const;

    void setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *destinationDataSource() const;
    IOntologyDelegate *destinationDelegate() const;

    void setProblemsOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    IOntologyDataSource *problemsDataSource() const;
    IOntologyDelegate *problemsDelegate() const;

  signals:
    void dataChangedSignal();

  public slots:
    void dataChangedSlot();
};

#endif // LOGICALINFERENCE_H
