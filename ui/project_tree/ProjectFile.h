#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "lib_json/json/json.h"

#include "../../core/OntologyDataController.h"
#include "ProjectFileCategory.h"

class ProjectFile {

  private:
    QString m_name;
    QString m_path;

    OntologyDataController *m_ontologyController;
    QList<ProjectFileCategory *> m_categories;

  public:
    ProjectFile(const QString &path, const QString &name);
    ~ProjectFile();

    const QString &name() const;
    const QString &path() const;

    OntologyDataController *ontologyController() const;

    void addCategory(ProjectFileCategory *category);
    void removeCategory(ProjectFileCategory *category);
    ProjectFileCategory *getCategoryByName(const QString &name) const;
    QList<ProjectFileCategory*> categories() const;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);
};

#endif // PROJECTFILE_H
