#include "OntologyGraphElement.h"

#include <QDebug>
#include <QSTring>

OntologyGraphElement::OntologyGraphElement() {

  m_id = -1;
  m_name = "";
}

QString OntologyGraphElement::attributesAsText() const {

  if (m_attributes.size() > 0) {
    Json::Value jsonValue;
    foreach (QString key, m_attributes.keys()) {
      QString value = m_attributes.value(key);
      jsonValue[key.toStdString()] = Json::Value(value.toStdString());
    }

    return QString::fromStdString(jsonValue.toStyledString());
  }
  else {
    return QString();
  }
}

void OntologyGraphElement::setAttributes(const QString &text) {

  qDebug() << text;

  Json::Reader reader;
  Json::Value jsonValue;
  bool ok = reader.parse(text.toStdString(), jsonValue);

  if (ok) {
    m_attributes.clear();

    for (int i = 0; i < jsonValue.size(); ++i) {
      QString key = QString::fromStdString(jsonValue.getMemberNames().at(i));
      QString value = QString::fromStdString(jsonValue[jsonValue.getMemberNames().at(i)].asString());
      m_attributes.insert(key, value);
    }

    attributesChanged();
  }
}

Json::Value OntologyGraphElement::jsonRepresentation() const {

  Json::Value value;
  value["id"] = Json::Value((Json::Int64)m_id);
  return value;
}

void OntologyGraphElement::setId(long id) {

  m_id = id;
}

long OntologyGraphElement::id() const {

  return m_id;
}

void OntologyGraphElement::setName(const QString &name) {

  m_name = name;
}

QString OntologyGraphElement::name() const {

  return m_name;
}
