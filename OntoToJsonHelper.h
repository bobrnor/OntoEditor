#ifndef ONTOTOJSONHELPER_H
#define ONTOTOJSONHELPER_H

#include "lib_json/json/json.h"
#include <QList>

class IOntologyDataSource;
class NodeData;

class OntoToJsonHelper {
  private:
    IOntologyDataSource *m_ontoDataSource;

  protected:
    void putNodeIntoJson(NodeData *node, Json::Value *jsonValue) const;
    NodeData *rootNode(NodeData *node) const;
    QList<NodeData *> getAllInstances(NodeData *node) const;

  public:
    OntoToJsonHelper(IOntologyDataSource *dataSource);

    Json::Value generateJson();
};

#endif // ONTOTOJSONHELPER_H
