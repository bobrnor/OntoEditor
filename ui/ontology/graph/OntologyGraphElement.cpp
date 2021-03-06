#include "OntologyGraphElement.h"

#include <QDebug>
#include <QSTring>

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

void OntologyGraphElement::setRelatedDataController(OntologyDataController *dataController) {

  m_dataController = dataController;
}

OntologyDataController *OntologyGraphElement::relatedDataController() const {

  return m_dataController;
}
