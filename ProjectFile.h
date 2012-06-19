#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "OntologyDataController.h"
#include "ProjectFileCategory.h"

class ProjectFile {

  private:
    QString m_name;

    OntologyDataController *m_sourceOntologyController;
    OntologyDataController *m_destinationOntologyController;
    QList<ProjectFileCategory> m_categories;

  public:
    ProjectFile(QString name);
    ~ProjectFile();

    QString name() const;

    OntologyDataController *sourceOntologyController() const;
    OntologyDataController *destinationOntologyController() const;

    void addCategory(const ProjectFileCategory &category);
    void removeCategory(const ProjectFileCategory &category);
    QList<ProjectFileCategory> categories() const;
};

#endif // PROJECTFILE_H
