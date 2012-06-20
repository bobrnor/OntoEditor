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

void ProjectFile::addCategory(ProjectFileCategory *category) {

  foreach (ProjectFileCategory *existsCategory, m_categories) {
    if (category->name() == existsCategory->name()) {
      return;
    }
  }

  m_categories.append(category);
}

void ProjectFile::removeCategory(ProjectFileCategory *category) {

  m_categories.removeOne(category);
}

ProjectFileCategory *ProjectFile::getCategoryByName(const QString &name) const {

  foreach (ProjectFileCategory *category, m_categories) {
    if (category->name() == name) {
      return category;
    }
  }
  return NULL;
}

QList<ProjectFileCategory*> ProjectFile::categories() const {

  return m_categories;
}
