#ifndef RELATIONVISUALIZEDLINE_H
#define RELATIONVISUALIZEDLINE_H

#include "RelationItem.h"

class NodeItem;

class RelationVisualizedLine : public RelationItem {
  private:
    NodeItem *m_sourceNode;
    QPointF m_endPoint;
    bool m_endPointEmpty;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  public:
    explicit RelationVisualizedLine(QGraphicsItem *parent = 0);

    void adjust();
    void removeFromNodes();

    void setSourceNode(NodeItem *node);
    NodeItem *sourceNode() const;

    void setEndPoint(QPointF point);
    QPointF endPoint() const;    
};

#endif // RELATIONVISUALIZEDLINE_H
