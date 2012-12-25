#ifndef RELATIONITEM_H
#define RELATIONITEM_H

#include <QGraphicsLineItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"
#include "core/IOntologyDataSource.h"

class NodeItem;

class RelationItem : public QGraphicsLineItem, public OntologyGraphElement {
  private:
    NodeItem *m_sourceNode;
    NodeItem *m_destinationNode;

    IOntologyDataSource *m_dataSource;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

  public:
    explicit RelationItem(QGraphicsItem *parent = 0);
    ~RelationItem();

    QRectF boundingRect() const;

    virtual void adjust();

    virtual void removeFromNodes();

    virtual void setSourceNode(NodeItem *node);
    virtual NodeItem *sourceNode() const;

    virtual void setDestinationNode(NodeItem *node);
    virtual NodeItem *destinationNode() const;

    void setRelatedDataSource(IOntologyDataSource *dataSource);
};

#endif // RELATIONITEM_H
