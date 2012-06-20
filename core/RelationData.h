#ifndef RELATIONDATA_H
#define RELATIONDATA_H

#include <QString>

struct RelationData {
    long id;
    long sourceNodeId;
    long destinationNodeId;
    QString name;
};

#endif // RELATIONDATA_H
