#ifndef PROJECTTREECONTROLLER_H
#define PROJECTTREECONTROLLER_H

#include "ProjectTreeWidget.h"
#include "ProjectTreeWidgetDelegate.h"

class Project;
class ProjectFileCategory;

class ProjectTreeViewController : public QObject, public ProjectTreeWidgetDelegate {

    Q_OBJECT

  private:
    ProjectTreeWidget *m_treeWidget;
    Project *m_project;

    void setupMenu();

  public:
    ProjectTreeViewController();
    ~ProjectTreeViewController();

    void setProject(Project *project);
    Project *project() const;

    QTreeWidget *widget() const;

    void updateData();

    bool isCategoryIndex(const QModelIndex &index);
    bool isFileIndex(const QModelIndex &index);
    bool processDrop(QTreeWidgetItem *parentItem, const QMap<int, QVariant> &dropData);

  public slots:
    void itemSelectedSlot(const QModelIndex &index);
    void addCategorySlot();

  signals:
    void currentFileChangedSignal(const QString &fileName);
    void categorySelectedSignal(const QString &fileName, const QString &categoryName);
};

#endif // PROJECTTREECONTROLLER_H
