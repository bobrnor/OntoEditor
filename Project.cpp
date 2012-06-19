#include "Project.h"

#include <fstream>

#include "JsonToOntoHelper.h"
#include "OntoToJsonHelper.h"

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

OntologyDataController *Project::problemsOntology() const {

  return m_problemsOntologyController;
}

QString Project::findCorrenspondingLanguage(const QString &term) const {

  foreach (QString languageName, m_languageOntologies.keys()) {
    IOntologyDataSource *languageDataSource = m_languageOntologies.value(languageName);
    NodeData *node = languageDataSource->findNode(term);
    if (node != NULL) {
      return languageName;
    }
  }
  return QString::null;
}

bool Project::importSourceFile(const QString &path) {

  if (QFile::exists(path)) {
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
        QFileInfo fileInfo(path);
        ProjectFile *file = new ProjectFile(fileInfo.fileName());
        m_files.insert(file->name(), file);

        JsonToOntoHelper jtoHelper;
        jtoHelper.setLanguageOntology(m_languageOntologies.value(language), m_languageOntologies.value(language));
        jtoHelper.setDestinationOntology(file->sourceOntologyController(), file->sourceOntologyController());
        jtoHelper.fillOntology(jsonState);

        return true;
      }
    }
  }

  return false;
}

bool Project::exportDestinationFile(const QString &fileName, const QString &path) {

  if (m_files.contains(fileName)) {
    ProjectFile *projectFile = m_files.value(fileName);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      OntoToJsonHelper otjHelper(projectFile->destinationOntologyController());
      Json::Value json = otjHelper.generateJson();

      QTextStream stream(&file);
      stream.setCodec("UTF-8");
      stream.setAutoDetectUnicode(true);
      stream << QString::fromStdString(json.toStyledString());

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

bool Project::openProject(const QString &path) {

  return false;
}

bool Project::saveProject(const QString &path) {

  return false;
}
