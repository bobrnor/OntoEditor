#include "OntologyGraphElement.h"

OntologyGraphElement::OntologyGraphElement() {

  m_id = -1;
  m_name = "";
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
