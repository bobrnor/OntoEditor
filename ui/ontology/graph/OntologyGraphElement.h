#ifndef ONTOLOGYGRAPHELEMENT_H
#define ONTOLOGYGRAPHELEMENT_H

#include <QString>
#include <QMap>
#include "lib_json/json/json.h"

#include "core/OntologyDataController.h"

class OntologyGraphElement {
  protected:
    long m_id;
    QString m_name;

    OntologyDataController *m_dataController;

    virtual void attributesChanged() = 0;

  public:
    OntologyGraphElement();

    void setId(long id);
    long id() const;

    virtual QString attributesAsText() const = 0;
    virtual QMap<QString, QString> attributest() const = 0;
    virtual void setAttributes(const QString &text) = 0;

    void setName(const QString &name);
    QString name() const;

    void setRelatedDataController(OntologyDataController *dataController);
    OntologyDataController *relatedDataController() const;

    Json::Value jsonRepresentation() const;
};

#endif // ONTOLOGYGRAPHELEMENT_H
