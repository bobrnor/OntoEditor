#ifndef PROJECTTREEWIDGET_H
#define PROJECTTREEWIDGET_H

#include <QTreeWidget>

#include "ProjectTreeWidgetDelegate.h"

class ProjectTreeWidget : public QTreeWidget {

    Q_OBJECT

  private:
    ProjectTreeWidgetDelegate *m_delegate;

  protected:
    bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);

  public:
    explicit ProjectTreeWidget(QWidget *parent = 0);

    void setDelegate(ProjectTreeWidgetDelegate *delegate);
    ProjectTreeWidgetDelegate *delegate() const;
    
  signals:
    
  public slots:
    
};

#endif // PROJECTTREEWIDGET_H
