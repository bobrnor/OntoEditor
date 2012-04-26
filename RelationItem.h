#ifndef RELATIONITEM_H
#define RELATIONITEM_H

#include <QGraphicsLineItem>

class NodeItem;

class RelationItem : public QGraphicsLineItem {
  private:
    NodeItem *m_sourceNode;
    NodeItem *m_destinationNode;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  public:
    explicit RelationItem(QGraphicsItem *parent = 0);

    void adjust();

    void setSourceNode(NodeItem *node);
    NodeItem *sourceNode() const;

    void setDestinationNode(NodeItem *node);
    NodeItem *destinationNode() const;
    
  signals:
    
  public slots:
    
};

#endif // RELATIONITEM_H
