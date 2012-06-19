#ifndef PROJECTTREEWIDGETDELEGATE_H
#define PROJECTTREEWIDGETDELEGATE_H

#include <QMap>
#include <QVariant>

class ProjectTreeWidgetDelegate {

  public:
    virtual bool processDrop(QTreeWidgetItem *parentItem, const QMap<int, QVariant> &dropData) = 0;
};

#endif // PROJECTTREEWIDGETDELEGATE_H
