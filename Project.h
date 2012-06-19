#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

#include "OntologyDataController.h"
#include <ProjectFile.h>

class Project {

  private:
    QMap<QString, ProjectFile *> m_files;
    QMap<QString, OntologyDataController*> m_languageOntologies;
    OntologyDataController *m_problemsOntologyController;

    QString findCorrenspondingLanguage(const QString &term) const;

  public:
    Project();
    ~Project();

    void addLanguageOntology(const QString &languageName, OntologyDataController *ontologyController);
    void removeLanguageOntology(const QString &languageName);
    QList<QString> availableLanguages() const;
    OntologyDataController *getLanguageOntologyByName(const QString &languageName) const;

    OntologyDataController *problemsOntology() const;

    bool importSourceFile(const QString &path);
    bool exportDestinationFile(const QString &fileName, const QString &path);
    ProjectFile *getProjectFileByName(const QString &name) const;

    bool openWorkspace(const QString &path);
    bool saveWorkspace(const QString &path);

    bool openProject(const QString &path);
    bool saveProject(const QString &path);
};

#endif // PROJECT_H
