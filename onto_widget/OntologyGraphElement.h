#ifndef ONTOLOGYGRAPHELEMENT_H
#define ONTOLOGYGRAPHELEMENT_H

#include <QString>
#include "lib_json/json/json.h"

class OntologyGraphElement {
  protected:
    long m_id;
    QString m_name;

  public:
    OntologyGraphElement();
    OntologyGraphElement(const Json::Value &jsonValue);

    void setId(long id);
    long id() const;

    void setName(const QString &name);
    QString name() const;

    Json::Value jsonRepresentation() const;
};

#endif // ONTOLOGYGRAPHELEMENT_H
