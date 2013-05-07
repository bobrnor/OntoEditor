#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

#include "lib_json/json/json.h"

#include "../../core/OntologyDataController.h"
#include "ProjectFile.h"

class Project {

  private:
    QList<ProjectFile *> m_files;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);

  public:
    Project();
    ~Project();

    ProjectFile *openFile(const QString &path);
    bool saveFile(const ProjectFile *file, const QString &path);
    bool saveFile(const ProjectFile *file);
    ProjectFile *getProjectFileByIndex(int index) const;
    ProjectFile *getProjectFileByName(const QString &name) const;
//    ProjectFile *getProjectFileByName(const QString &name) const;
//    QList<QString> availableFileNames() const;
    int filesCount() const;

    bool openProject(const QString &path);
    bool saveProject(const QString &path);
};

#endif // PROJECT_H
