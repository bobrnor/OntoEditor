#ifndef JSONTOONTOHELPER_H
#define JSONTOONTOHELPER_H

#include "lib_json/json/json.h"

class IOntologyDelegate;
class IOntologyDataSource;
class NodeData;

class JsonToOntoHelper {

  private:
    IOntologyDataSource *m_languageDataSource;
    IOntologyDelegate *m_languageDelegate;

    IOntologyDataSource *m_destinationDataSource;
    IOntologyDelegate *m_destinationDelegate;

  protected:
    void process(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processArrayValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processJsonValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);
    void processValue(const Json::Value &jsonValue, NodeData *currentLanguageNode, NodeData *currentDestinationNode);

    NodeData *copyNode(NodeData *currentLanguageNode, NodeData *currentDestinationNode);

  public:
    JsonToOntoHelper();

    void setLanguageOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);
    void setDestinationOntology(IOntologyDataSource *dataSource, IOntologyDelegate *delegate);

    void fillOntology(const Json::Value &json);
};

#endif // JSONTOONTOHELPER_H
