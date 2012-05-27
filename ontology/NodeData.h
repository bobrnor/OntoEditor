#ifndef NODEDATA_H
#define NODEDATA_H

#include <QString>
#include <QList>

struct NodeData {
    long id;
    QString name;
    QList<long> relations;
};

#endif // NODEDATA_H
