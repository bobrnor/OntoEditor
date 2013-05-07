#include "Project.h"

#include <fstream>

#include "../../core/JsonToOntoHelper.h"
#include "../../core/OntoToJsonHelper.h"

Project::Project() {

}

Project::~Project() {

  foreach (ProjectFile *file, m_files) {
    delete file;
  }
}

ProjectFile *Project::getProjectFileByName(const QString &name) const {

  if (m_files.contains(name)) {
    return m_files.value(name);
  }
  else {
    return NULL;
  }
}

QList<QString> Project::availableFileNames() const {

  qDebug() << m_files.keys();

  return m_files.keys();
}
int Project::filesCount() const {

  return m_files.count();
}

Json::Value Project::serialize() const {

  Json::Value json;
  Json::Value filesJson = Json::Value(Json::arrayValue);
  foreach (ProjectFile *file, m_files.values()) {
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
    ProjectFile *file = new ProjectFile("");
    file->deserialize(filesJson[i]);
    m_files.insert(file->name(), file);
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
