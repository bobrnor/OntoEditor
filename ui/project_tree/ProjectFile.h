#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "lib_json/json/json.h"

#include "../../core/OntologyDataController.h"
#include "ProjectFileCategory.h"

class ProjectFile {

  private:
    QString m_name;

    OntologyDataController *m_ontologyController;
    QList<ProjectFileCategory *> m_categories;

  public:
    ProjectFile(QString name);
    ~ProjectFile();

    QString name() const;

    OntologyDataController *ontologyController() const;

    void addCategory(ProjectFileCategory *category);
    void removeCategory(ProjectFileCategory *category);
    ProjectFileCategory *getCategoryByName(const QString &name) const;
    QList<ProjectFileCategory*> categories() const;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);
};

#endif // PROJECTFILE_H
