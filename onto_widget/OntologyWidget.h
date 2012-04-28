#ifndef ONTOLOGYWIDGET_H
#define ONTOLOGYWIDGET_H

#include "lib_json/json/json.h"

#include "OntologyGraphicsView.h"
#include "IOntologyWidgetDelegate.h"
#include "IOntologyDataSource.h"

class RelationVisualizedLine;
class NodeItem;

namespace Ui {
  class OntologyWidget;
}

class OntologyWidget : public QWidget {
    Q_OBJECT
    
  public:
    explicit OntologyWidget(QWidget *parent = 0);
    ~OntologyWidget();

    void setDelegate(IOntologyWidgetDelegate *delegate);
    IOntologyWidgetDelegate *delegate() const;

    void setDataSource(IOntologyDataSource *dataSource);
    IOntologyDataSource *dataSource() const;

    Json::Value serialize() const;
    void deserialize(const Json::Value &json);
    
  private:
    Ui::OntologyWidget *ui;

    OntologyGraphicsView *m_ontologyView;
    QPointF m_lastRightClickScenePosition;
    RelationVisualizedLine *m_relationVisualizedLine;
    IOntologyDataSource *m_dataSource;
    IOntologyWidgetDelegate *m_delegate;
    QMap<long, QPointF> m_nodePositions;

    bool m_editRelationMode;

    void setEditRelationMode(bool on);
    void setRelation(NodeItem *sourceNode, NodeItem *destinationNode);

    void updateData();

  public slots:
    void showContextMenuSlot(const QPoint &pos);

    void addNodeSlot();
    void setRelationSlot();
    void editNodeSlot();
    void editRelationSlot();
    void removeSelectedSlot();

    void sceneSelectionChangedSlot();

    void ontologyViewMousePositionChangedSlot(const QPoint &pos);
};

#endif // ONTOLOGYWIDGET_H
