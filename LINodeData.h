#ifndef LINODEDATA_H
#define LINODEDATA_H

#include <QString>
#include "ontology/RelationData.h"

struct LINodeData {
    long id;
    QString name;
    QList<RelationData *> relations;
};

#endif // LINODEDATA_H
