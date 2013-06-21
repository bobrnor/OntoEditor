#ifndef RELATIONDATA_H
#define RELATIONDATA_H

#include <QString>
#include <QMap>

struct RelationData {
    long id;
    long sourceNodeId;
    long destinationNodeId;
    QString name;
    QMap<QString, QString> attributes;
};

#endif // RELATIONDATA_H
