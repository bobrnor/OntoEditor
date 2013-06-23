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

Json::Value ProjectFile::serialize() const {

  Json::Value json;
  json["name"] = Json::Value(m_name.toStdString());
  json["path"] = Json::Value(m_path.toStdString());
  json["ontology"] = m_ontologyController->serialize();

  Json::Value categoriesJson = Json::Value(Json::arrayValue);
  foreach (ProjectFileCategory *category, m_categories) {
    categoriesJson.append(category->serialize());
  }

  json["categories"] = categoriesJson;

  return json;
}

void ProjectFile::deserialize(const Json::Value &json) {

  m_name = QString::fromStdString(json["name"].asString());
  m_path = QString::fromStdString(json["path"].asString());
  m_ontologyController->deserialize(json["ontology"]);

  m_categories.clear();

  Json::Value categoriesJson = json["categories"];
  int categoriesCount = categoriesJson.size();
  for (int i = 0; i < categoriesCount; ++i) {
    ProjectFileCategory *category = new ProjectFileCategory("", this);
    category->deserialize(categoriesJson[i], this);
    m_categories.append(category);
  }
}
