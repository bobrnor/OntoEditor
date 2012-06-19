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

bool ProjectFileCategory::operator ==(const ProjectFileCategory &category) {

  return m_name == category.name();
}
