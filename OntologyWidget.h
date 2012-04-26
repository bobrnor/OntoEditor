#ifndef ONTOLOGYWIDGET_H
#define ONTOLOGYWIDGET_H

#include "OntologyGraphicsView.h"

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
    
  private:
    Ui::OntologyWidget *ui;

    OntologyGraphicsView *m_ontologyView;
    QPointF m_lastRightClickScenePosition;
    RelationVisualizedLine *m_relationVisualizedLine;

    bool m_editRelationMode;

    void setEditRelationMode(bool on);
    void setRelation(NodeItem *sourceNode, NodeItem *destinationNode);

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
