#ifndef ONTOLOGYTREEVIEWCONTROLLER_H
#define ONTOLOGYTREEVIEWCONTROLLER_H

#include <QtGui>

#include "../IOntologyDataSource.h"
#include "../IOntologyDelegate.h"

class OntologyTreeViewController : public QObject {
    Q_OBJECT

  private:
    QTreeView *m_objectsTreeView;
    QStandardItemModel *m_objectsModel;
    IOntologyDataSource *m_dataSource;
    IOntologyDelegate *m_delegate;

  public:
    OntologyTreeViewController();
    ~OntologyTreeViewController();

    QTreeView *treeView() const;

    void setDataSource(IOntologyDataSource *dataSource);
    IOntologyDataSource *dataSource() const;

    void setDelegate(IOntologyDelegate *delegate);
    IOntologyDelegate *delegate() const;

    void updateData();

  signals:
    void dataChangedSignal();
    void itemSelectedSignal(long id);

  public slots:
    void dataChangedSlot();
    void itemSelectedSlot(const QModelIndex &index);
};

#endif // ONTOLOGYTREEVIEWCONTROLLER_H
