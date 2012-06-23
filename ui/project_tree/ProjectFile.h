#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "lib_json/json/json.h"

#include "../../core/OntologyDataController.h"
#include "ProjectFileCategory.h"

class ProjectFile {

  private:
    QString m_name;

    OntologyDataController *m_sourceOntologyController;
    OntologyDataController *m_destinationOntologyController;
    QList<ProjectFileCategory *> m_categories;
    QString m_languageName;

  public:
    ProjectFile(QString name);
    ~ProjectFile();

    QString name() const;
    void setLanguageName(const QString &languageName);
    QString languageName() const;

    OntologyDataController *sourceOntologyController() const;
    OntologyDataController *destinationOntologyController() const;

    void addCategory(ProjectFileCategory *category);
    void removeCategory(ProjectFileCategory *category);
    ProjectFileCategory *getCategoryByName(const QString &name) const;
    QList<ProjectFileCategory*> categories() const;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);
};

#endif // PROJECTFILE_H
