#include "Project.h"

#include <fstream>

#include "../../core/JsonToOntoHelper.h"
#include "../../core/OntoToJsonHelper.h"

Project::Project() {

  m_problemsOntologyController = NULL;
}

Project::~Project() {

  foreach (ProjectFile *file, m_files) {
    delete file;
  }

  foreach (OntologyDataController *controller, m_languageOntologies.values()) {
    delete controller;
  }

  delete m_problemsOntologyController;
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

void Project::addLanguageOntology(const QString &languageName, OntologyDataController *ontologyController) {

  m_languageOntologies.insert(languageName, ontologyController);
}

void Project::removeLanguageOntology(const QString &languageName) {

  m_languageOntologies.remove(languageName);
}

QList<QString> Project::availableLanguages() const {

  return m_languageOntologies.keys();
}

OntologyDataController *Project::getLanguageOntologyByName(const QString &languageName) const {

  if (m_languageOntologies.contains(languageName)) {
    return m_languageOntologies.value(languageName);
  }
  else {
    return NULL;
  }
}

OntologyDataController *Project::problemsOntologyController() const {

  return m_problemsOntologyController;
}

QString Project::findCorrenspondingLanguage(const QString &term) const {

  foreach (QString languageName, m_languageOntologies.keys()) {
    OntologyDataController *languageOntologyDataController = m_languageOntologies.value(languageName);
    NodeData *node = languageOntologyDataController->findNode(term);
    if (node != NULL) {
      return languageName;
    }
  }
  return QString::null;
}

QString Project::generateDestinationCode(const QString &fileName) const {

  ProjectFile *projectFile = m_files.value(fileName);
  OntoToJsonHelper otjHelper(projectFile->destinationOntologyController());
  Json::Value json = otjHelper.generateJson();
  return QString::fromStdString(json.toStyledString());
}

bool Project::importSourceFile(const QString &path) {

  if (QFile::exists(path)) {
    QFileInfo fileInfo(path);
    if (!m_files.contains(fileInfo.fileName())) {
      Json::Reader reader;
      std::ifstream fileStream;
      fileStream.open(path.toStdString().c_str());

      Json::Value jsonState;
      bool ok = reader.parse(fileStream, jsonState);

      Q_ASSERT(ok);

      if (ok) {
        QString term = QString::fromStdString(jsonState.getMemberNames().at(0));
        QString language = findCorrenspondingLanguage(term);

        if (!language.isNull()) {
          ProjectFile *file = new ProjectFile(fileInfo.fileName());
          file->setLanguageName(language);
          m_files.insert(file->name(), file);

          JsonToOntoHelper jtoHelper;
          jtoHelper.setLanguageOntology(m_languageOntologies.value(language));
          jtoHelper.setDestinationOntology(file->sourceOntologyController());
          jtoHelper.fillOntology(jsonState);

          file->sourceOntologyController()->setSourceCode(QString::fromStdString(jsonState.toStyledString()));

          return true;
        }
      }
    }
  }

  return false;
}

bool Project::exportDestinationFile(const QString &fileName, const QString &path) {

  if (m_files.contains(fileName)) {
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QString code = generateDestinationCode(fileName);

      QTextStream stream(&file);
      stream.setCodec("UTF-8");
      stream.setAutoDetectUnicode(true);
      stream << code;

      return true;
    }
  }

  return false;
}

bool Project::openWorkspace(const QString &path) {

  if (QFile::exists(path)) {
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(path.toStdString().c_str());

    Json::Value jsonState;
    bool ok = reader.parse(fileStream, jsonState);
    if (ok) {
      OntologyDataController *javaOntologyController = new OntologyDataController(jsonState["java"]);
      OntologyDataController *objcOntologyController = new OntologyDataController(jsonState["objc"]);
      m_problemsOntologyController = new OntologyDataController(jsonState["problems"]);

      m_languageOntologies.insert("java", javaOntologyController);
      m_languageOntologies.insert("objc", objcOntologyController);

      return true;
    }
  }

  return false;
}

bool Project::saveWorkspace(const QString &path) {

  QFile file(path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Json::Value jsonState;
    jsonState["java"] = m_languageOntologies.value("java")->serialize();
    jsonState["objc"] = m_languageOntologies.value("objc")->serialize();
    jsonState["problems"] = m_problemsOntologyController->serialize();

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);
    stream << QString::fromStdString(jsonState.toStyledString());
    return true;
  }
  return false;
}

Json::Value Project::serialize() const {

  Json::Value json;
  Json::Value filesJson = Json::Value(Json::arrayValue);
  foreach (ProjectFile *file, m_files.values()) {
    filesJson.append(file->serialize());
  }
  json["files"] = filesJson;

  Json::Value languageOntologiesJson;
  foreach (QString languageName, m_languageOntologies.keys()) {
    OntologyDataController *languageOntology = m_languageOntologies.value(languageName);
    languageOntologiesJson[languageName.toStdString()] = languageOntology->serialize();
  }
  json["language_ontologies"] = languageOntologiesJson;

  json["problems_ontology"] = m_problemsOntologyController->serialize();

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

  m_languageOntologies.clear();

  Json::Value languageOntologiesJson = json["language_ontologies"];
  std::vector<std::string> members = languageOntologiesJson.getMemberNames();
  for (int i = 0; i < members.size(); ++i) {
    std::string member = members.at(i);
    OntologyDataController *languageOntology = new OntologyDataController(languageOntologiesJson[member]);
    m_languageOntologies.insert(QString::fromStdString(member), languageOntology);
  }

  m_problemsOntologyController = new OntologyDataController(json["problems_ontology"]);
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
