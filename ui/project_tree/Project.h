#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

#include "../../core/OntologyDataController.h"
#include "ProjectFile.h"

class Project {

  private:
    QList<ProjectFile *> m_files;

    QVariant serialize() const;
    void deserialize(const QVariant &json);

  public:
    Project();
    ~Project();

    ProjectFile *createFile(const QString &jsonString);
    ProjectFile *openFile(const QString &path);
    bool saveFile(ProjectFile *file, const QString &path);
    bool saveFile(ProjectFile *file);
    ProjectFile *getProjectFileByIndex(int index) const;
    ProjectFile *getProjectFileByName(const QString &name) const;
    int filesCount() const;

    bool openProject(const QString &path);
    bool saveProject(const QString &path);
};

#endif // PROJECT_H
