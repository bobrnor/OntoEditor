#include "Project.h"

#include <fstream>

Project::Project() {

}

Project::~Project() {

  foreach (ProjectFile *file, m_files) {
    delete file;
  }
}

ProjectFile *Project::openFile(const QString &path) {

  if (QFile::exists(path)) {
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(path.toStdString().c_str());

    Json::Value jsonState;
    bool ok = reader.parse(fileStream, jsonState);
    if (ok) {
      QFileInfo fileInfo(path);
      ProjectFile *newFile = new ProjectFile(path, fileInfo.fileName());
      newFile->ontologyController()->deserialize(jsonState);
      m_files.append(newFile);
      return newFile;
    }
  }

  return NULL;
}

bool Project::saveFile(const ProjectFile *file, const QString &path) {

  if (file != NULL) {
    QFile dstFile(path);
    if (dstFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      Json::Value jsonState = file->serialize();

      QTextStream stream(&dstFile);
      stream.setCodec("UTF-8");
      stream.setAutoDetectUnicode(true);
      stream << QString::fromStdString(jsonState.toStyledString());
      return true;
    }
  }

  return false;
}

bool Project::saveFile(const ProjectFile *file) {

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

Json::Value Project::serialize() const {

  Json::Value json;
  Json::Value filesJson = Json::Value(Json::arrayValue);
  foreach (ProjectFile *file, m_files) {
    filesJson.append(file->serialize());
  }
  json["files"] = filesJson;

  return json;
}

void Project::deserialize(const Json::Value &json) {

  m_files.clear();

  Json::Value filesJson = json["files"];
  int filesCount = filesJson.size();
  for (int i = 0; i < filesCount; ++i) {
    ProjectFile *file = new ProjectFile("", "");
    file->deserialize(filesJson[i]);
    m_files.append(file);
  }
}

bool Project::openProject(const QString &path) {

  if (QFile::exists(path)) {
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(path.toStdString().c_str());

    Json::Value jsonState;
    bool ok = reader.parse(fileStream, jsonState);
    if (ok) {
      deserialize(jsonState);
      return true;
    }
  }

  return false;
}

bool Project::saveProject(const QString &path) {

  QFile file(path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Json::Value jsonState = serialize();

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);
    stream << QString::fromStdString(jsonState.toStyledString());
    return true;
  }
  return false;
}
