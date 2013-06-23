#include "ProjectFile.h"

ProjectFile::ProjectFile() : m_path(""), m_name("Untitled") {

  m_ontologyController = new OntologyDataController();
}

ProjectFile::ProjectFile(const QString &path, const QString &name) : m_path(path), m_name(name) {

  m_ontologyController = new OntologyDataController();
}

ProjectFile::~ProjectFile() {

  delete m_ontologyController;
}

const QString &ProjectFile::name() const {

  return m_name;
}

const QString &ProjectFile::path() const {

  return m_path;
}

void ProjectFile::setName(const QString &name) {

  m_name = name;
}

void ProjectFile::setPath(const QString &path) {

  m_path = path;
}

OntologyDataController *ProjectFile::ontologyController() const {

  return m_ontologyController;
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

QVariant ProjectFile::serialize() const {

  QVariantMap jsonMap;
  jsonMap["name"] = m_name;
  jsonMap["path"] = m_path;
  jsonMap["ontology"] = m_ontologyController->serialize();

  QVariantList categoriesJson;
  foreach (ProjectFileCategory *category, m_categories) {
    categoriesJson.append(category->serialize());
  }
  jsonMap["categories"] = categoriesJson;
  return jsonMap;
}

void ProjectFile::deserialize(const QVariant &json) {

  QVariantMap jsonMap = json.toMap();

  m_name = jsonMap["name"].toString();
  m_path = jsonMap["path"].toString();
  m_ontologyController->deserialize(jsonMap["ontology"]);

  m_categories.clear();

  QVariantList categoriesJson = jsonMap["categories"].toList();
  foreach (QVariant categoryJson, categoriesJson) {
    ProjectFileCategory *category = new ProjectFileCategory("", this);
    category->deserialize(categoryJson, this);
    m_categories.append(category);
  }
}
