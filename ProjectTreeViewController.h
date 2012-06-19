#ifndef PROJECTTREECONTROLLER_H
#define PROJECTTREECONTROLLER_H

#include <QTreeWidget>

class Project;

class ProjectTreeViewController : public QObject {

    Q_OBJECT

  private:
    QTreeWidget *m_treeWidget;
    Project *m_project;

  public:
    ProjectTreeViewController();
    ~ProjectTreeViewController();

    void setProject(Project *project);
    Project *project() const;

    QTreeWidget *widget() const;

    void updateData();

  public slots:
    void itemSelectedSlot(const QModelIndex &index);

  signals:
    void currentFileChangedSignal(const QString &fileName);
};

#endif // PROJECTTREECONTROLLER_H
