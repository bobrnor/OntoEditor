#ifndef PROJECTFILECATEGORY_H
#define PROJECTFILECATEGORY_H

#include <QSet>
#include <QString>

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

    bool operator ==(const ProjectFileCategory &category);
};

#endif // PROJECTFILECATEGORY_H
