#include "Project.h"

#include <fstream>

Project::Project() {

}

Project::~Project() {

  foreach (ProjectFile *file, m_files) {
    delete file;
  }
}

ProjectFile *Project::createFile() {

  ProjectFile *newFile = new ProjectFile();
  m_files.append(newFile);
  return newFile;
}

ProjectFile *Project::createFile(const QString &jsonString) {

  if (jsonString.length() > 0) {
    QJson::Parser parser;
    bool ok = false;
    QVariant json = parser.parse(jsonString.toLocal8Bit(), &ok);
    if (ok) {
      ProjectFile *newFile = new ProjectFile();
      newFile->ontologyController()->deserialize(json);
      m_files.append(newFile);
      return newFile;
    }
  }

  return NULL;
}

ProjectFile *Project::openFile(const QString &path) {

  if (QFile::exists(path)) {
    QFile file(path);
    QJson::Parser parser;
    bool ok = false;
    QVariant json = parser.parse(&file, &ok);

    if (ok) {
      QFileInfo fileInfo(path);
      ProjectFile *newFile = new ProjectFile(path, fileInfo.fileName());
      newFile->ontologyController()->deserialize(json);
      m_files.append(newFile);
      return newFile;
    }
  }

  return NULL;
}

bool Project::saveFile(ProjectFile *file, const QString &path) {

  if (file != NULL) {
    QFile dstFile(path);
    if (dstFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QVariant json = file->ontologyController()->serialize();

      QJson::Serializer serializer;
      QByteArray data = serializer.serialize(json);

      dstFile.write(data);

      QFileInfo fileInfo(path);
      file->setName(fileInfo.fileName());
      file->setPath(path);

      return true;
    }
  }

  return false;
}

bool Project::saveFile(ProjectFile *file) {

  return saveFile(file, file->path());
}

ProjectFile *Project::getProjectFileByName(const QString &name) const {

  foreach (ProjectFile *file, m_files) {
    if (file->name() == name) {
      return file;
    }
  }

  return NULL;
}

ProjectFile *Project::getProjectFileByIndex(int index) const {

  return m_files[index];
}

int Project::filesCount() const {

  return m_files.count();
}

QVariant Project::serialize() const {

  QVariantList filesJson;
  foreach (ProjectFile *file, m_files) {
    filesJson.append(file->serialize());
  }

  QVariantMap json;
  json["files"] = filesJson;
  return json;
}

void Project::deserialize(const QVariant &json) {

  m_files.clear();

  QVariantList filesJson = json.toMap()["files"].toList();
  foreach (QVariant fileJson, filesJson) {
    ProjectFile *file = new ProjectFile("", "");
    file->deserialize(fileJson);
    m_files.append(file);
  }
}

bool Project::openProject(const QString &path) {

  if (QFile::exists(path)) {
    QFile file(path);
    QJson::Parser parser;
    bool ok = false;
    QVariant json = parser.parse(&file, &ok);

    if (ok) {
      deserialize(json);
      return true;
    }
  }

  return false;
}

bool Project::saveProject(const QString &path) {

  QFile file(path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QVariant jsonState = serialize();

    QJson::Serializer serializer;
    QByteArray data = serializer.serialize(jsonState);
    file.write(data);
    return true;
  }
  return false;
}
