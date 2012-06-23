#include "ProjectFileCategory.h"

ProjectFileCategory::ProjectFileCategory(QString name, ProjectFile *parentFile)
  : m_name(name), m_parentFile(parentFile) {

}

void ProjectFileCategory::setName(QString name) {

  m_name = name;
}

QString ProjectFileCategory::name() const {

  return m_name;
}

bool ProjectFileCategory::addRelatedNodeId(long id) {

  if (!m_relatedNodeIds.contains(id)) {
    m_relatedNodeIds.insert(id);
    return true;
  }
  return false;
}

void ProjectFileCategory::setRelatedNodeIds(QSet<long> ids) {

  m_relatedNodeIds = ids;
}

QSet<long> ProjectFileCategory::relatedNodeIds() const {

  return m_relatedNodeIds;
}

Json::Value ProjectFileCategory::serialize() const {

  Json::Value json;
  json["name"] = Json::Value(m_name.toStdString());

  Json::Value ids = Json::Value(Json::arrayValue);
  foreach (long id, m_relatedNodeIds) {
    ids.append(Json::Value((int64_t)id));
  }

  json["ids"] = ids;

  return json;
}

void ProjectFileCategory::deserialize(const Json::Value &json, ProjectFile *parentFile) {

  m_parentFile = parentFile;
  m_name = QString::fromStdString(json["name"].asString());

  m_relatedNodeIds.clear();

  Json::Value idsJson = json["ids"];
  int idsCount =idsJson.size();
  for (int i = 0; i < idsCount; ++i) {
    long id = idsJson[i].asLargestInt();
    m_relatedNodeIds.insert(id);
  }
}

bool ProjectFileCategory::operator ==(const ProjectFileCategory &category) {

  return m_name == category.name();
}
