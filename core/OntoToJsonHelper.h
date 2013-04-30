#ifndef ONTOTOJSONHELPER_H
#define ONTOTOJSONHELPER_H

#include "lib_json/json/json.h"
#include <QList>

class OntologyDataController;
class NodeData;

class OntoToJsonHelper {
  private:
    OntologyDataController *m_dataController;

  protected:
    void putNodeIntoJson(NodeData *node, Json::Value *jsonValue) const;
    NodeData *rootNode(NodeData *node) const;
    QList<NodeData *> getAllInstances(NodeData *node) const;

  public:
    OntoToJsonHelper(OntologyDataController *dataController);

    Json::Value generateJson();
};

#endif // ONTOTOJSONHELPER_H
