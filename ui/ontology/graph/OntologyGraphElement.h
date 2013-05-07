#ifndef ONTOLOGYGRAPHELEMENT_H
#define ONTOLOGYGRAPHELEMENT_H

#include <QString>
#include <QMap>
#include "lib_json/json/json.h"

class OntologyGraphElement {
  protected:
    long m_id;
    QString m_name;
    QMap<QString, QString> m_attributes;

    virtual void attributesChanged() = 0;

  public:
    OntologyGraphElement();

    void setId(long id);
    long id() const;

    QString attributesAsText() const;
    void setAttributes(const QString &text);

    void setName(const QString &name);
    QString name() const;

    Json::Value jsonRepresentation() const;
};

#endif // ONTOLOGYGRAPHELEMENT_H
