#ifndef PROJECTFILECATEGORY_H
#define PROJECTFILECATEGORY_H

#include <QSet>
#include <QString>

#include "lib_json/json/json.h"

class ProjectFile;

class ProjectFileCategory {

  private:
    QString m_name;
    QSet<long> m_relatedNodeIds;
    ProjectFile *m_parentFile;

  public:
    ProjectFileCategory(QString name, ProjectFile *parentFile);

    void setName(QString name);
    QString name() const;

    bool addRelatedNodeId(long id);
    void setRelatedNodeIds(QSet<long> ids);
    QSet<long> relatedNodeIds() const;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json, ProjectFile *parentFile);

    bool operator ==(const ProjectFileCategory &category);
};

#endif // PROJECTFILECATEGORY_H
