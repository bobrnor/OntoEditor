#ifndef NODEDATA_H
#define NODEDATA_H

#include <QString>
#include <QList>
#include <QMap>

struct NodeData {
    long id;
    QString name;
    QList<long> relations;
    QMap<QString, QString> attributes;
};

#endif // NODEDATA_H
