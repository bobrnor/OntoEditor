#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "../../core/OntologyDataController.h"
#include "ProjectFileCategory.h"

class ProjectFile {

    friend class Project;

  private:
    QString m_name;
    QString m_path;

    OntologyDataController *m_ontologyController;
    QList<ProjectFileCategory *> m_categories;

    void setName(const QString &name);
    void setPath(const QString &path);

  public:
    ProjectFile();
    ProjectFile(const QString &path, const QString &name);
    ~ProjectFile();

    const QString &name() const;
    const QString &path() const;

    OntologyDataController *ontologyController() const;

    void addCategory(ProjectFileCategory *category);
    void removeCategory(ProjectFileCategory *category);
    ProjectFileCategory *getCategoryByName(const QString &name) const;
    QList<ProjectFileCategory*> categories() const;

    QVariant serialize() const;
    void deserialize(const QVariant &json);
};

#endif // PROJECTFILE_H
