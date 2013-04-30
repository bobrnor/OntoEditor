#ifndef JSONTOONTOHELPER_H
#define JSONTOONTOHELPER_H

#include "lib_json/json/json.h"

class OntologyDataController;
class NodeData;

class JsonToOntoHelper {

  private:
    OntologyDataController *m_languageOntologyDataController;
    OntologyDataController *m_destinationOntologyDataController;

  protected:
    void process(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processArrayValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processJsonValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);

    NodeData *copyNode(NodeData *currentLanguageNode, NodeData *currentDestinationNode);

  public:
    JsonToOntoHelper();

    void setLanguageOntology(OntologyDataController *dataController);
    void setDestinationOntology(OntologyDataController *dataController);

    void fillOntology(const Json::Value &json);
};

#endif // JSONTOONTOHELPER_H
