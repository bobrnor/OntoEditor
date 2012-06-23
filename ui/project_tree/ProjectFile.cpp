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

  return QString(m_name).append(" (").append(m_languageName).append(")");
}

void ProjectFile::setLanguageName(const QString &languageName) {

  m_languageName = languageName;
}

QString ProjectFile::languageName() const {

  return m_languageName;
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

Json::Value ProjectFile::serialize() const {

  Json::Value json;
  json["name"] = Json::Value(m_name.toStdString());
  json["source_ontology"] = m_sourceOntologyController->serialize();
  json["destination_ontology"] = m_destinationOntologyController->serialize();
  json["language_name"] = Json::Value(m_languageName.toStdString());

  Json::Value categoriesJson = Json::Value(Json::arrayValue);
  foreach (ProjectFileCategory *category, m_categories) {
    categoriesJson.append(category->serialize());
  }

  json["categories"] = categoriesJson;

  return json;
}

void ProjectFile::deserialize(const Json::Value &json) {

  m_name = QString::fromStdString(json["name"].asString());
  m_sourceOntologyController->deserialize(json["source_ontology"]);
  m_destinationOntologyController->deserialize(json["destination_ontology"]);
  m_languageName = QString::fromStdString(json["language_name"].asString());

  m_categories.clear();

  Json::Value categoriesJson = json["categories"];
  int categoriesCount = categoriesJson.size();
  for (int i = 0; i < categoriesCount; ++i) {
    ProjectFileCategory *category = new ProjectFileCategory("", this);
    category->deserialize(categoriesJson[i], this);
    m_categories.append(category);
  }
}
