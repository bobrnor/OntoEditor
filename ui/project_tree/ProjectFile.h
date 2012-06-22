#ifndef PROJECTFILE_H
#define PROJECTFILE_H

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
};

#endif // PROJECTFILE_H
