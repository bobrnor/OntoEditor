#ifndef ONTOLOGYTREEVIEWCONTROLLER_H
#define ONTOLOGYTREEVIEWCONTROLLER_H

#include <QtGui>

#include "../../../core/OntologyDataController.h"

struct TVNodeData {
    NodeData *nodeData;
    QMap<QString, QList<TVNodeData> > *childNodes;
};

class OntologyTreeViewController : public QObject {
    Q_OBJECT

  private:
    QTreeView *m_objectsTreeView;
    QStandardItemModel *m_objectsModel;
    OntologyDataController *m_dataController;

    QMap<long, TVNodeData> m_treeData;

    bool m_dragEnabled;

    void updateTreeDataBottomToTop();
    void updateTreeDataTopToBottom();
    void buildNodesTree(QStandardItem *rootItem, QList<TVNodeData> nodes, QSet<long> *seenNodeIds);
    void clearTreeView();

  public:
    OntologyTreeViewController();
    ~OntologyTreeViewController();

    QTreeView *treeView() const;

    void setDataController(OntologyDataController *dataController);
    OntologyDataController *dataController() const;

    void setDragEnabled(bool enabled);
    bool isDragEnabled() const;

    void updateData();

  signals:
    void dataChangedSignal();
    void itemSelectedSignal(long id);

  public slots:
    void dataChangedSlot();
    void itemSelectedSlot(const QModelIndex &index);
};

#endif // ONTOLOGYTREEVIEWCONTROLLER_H
