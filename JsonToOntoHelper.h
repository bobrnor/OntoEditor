#ifndef JSONTOONTOHELPER_H
#define JSONTOONTOHELPER_H

#include "lib_json/json/json.h"

class IOntologyDelegate;
class IOntologyDataSource;
class NodeData;

class JsonToOntoHelper {

  private:
    IOntologyDelegate *m_ontoDelegate;
    IOntologyDataSource *m_ontoDataSource;

  protected:
    void process(const Json::Value &jsonValue, NodeData *currentNode);
    void processArrayValue(const Json::Value &jsonValue, NodeData *currentNode);
    void processJsonValue(const Json::Value &jsonValue, NodeData *currentNode);
    void processValue(const Json::Value &jsonValue, NodeData *currentNode);

  public:
    JsonToOntoHelper(IOntologyDelegate *delegate, IOntologyDataSource *dataSource);

    void fillOntology(const Json::Value &json);
};

#endif // JSONTOONTOHELPER_H
