#include "ProjectFileCategory.h"

#include <QVariantMap>

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

QVariant ProjectFileCategory::serialize() const {

  QVariantMap jsonMap;
  jsonMap["name"] = m_name;

  QVariantList ids;
  foreach (long id, m_relatedNodeIds) {
    ids.append(QVariant::fromValue(id));
  }
  jsonMap["ids"] = ids;
  return jsonMap;
}

void ProjectFileCategory::deserialize(const QVariant &json, ProjectFile *parentFile) {

  QVariantMap jsonMap = json.toMap();
  m_parentFile = parentFile;
  m_name = jsonMap["name"].toString();

  m_relatedNodeIds.clear();

  QVariantList idsJson = jsonMap["ids"].toList();
  foreach (QVariant idJson, idsJson) {
    m_relatedNodeIds.insert(idJson.toLongLong());
  }
}

bool ProjectFileCategory::operator ==(const ProjectFileCategory &category) {

  return m_name == category.name();
}
