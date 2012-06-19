#include "ProjectFile.h"

ProjectFile::ProjectFile(QString name) : m_name(name) {

  m_sourceOntologyController = new OntologyDataController();
  m_destinationOntologyController = new OntologyDataController();
}

ProjectFile::~ProjectFile() {

  delete m_sourceOntologyController;
  delete m_destinationOntologyController;
}

QString ProjectFile::name() const {

  return m_name;
}

OntologyDataController *ProjectFile::sourceOntologyController() const {

  return m_sourceOntologyController;
}

OntologyDataController *ProjectFile::destinationOntologyController() const {

  return m_destinationOntologyController;
}

void ProjectFile::addCategory(const ProjectFileCategory &category) {

  m_categories.append(category);
}

void ProjectFile::removeCategory(const ProjectFileCategory &category) {

  m_categories.removeOne(category);
}

QList<ProjectFileCategory> ProjectFile::categories() const {

  return m_categories;
}
