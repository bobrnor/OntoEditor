#ifndef RELATIONITEM_H
#define RELATIONITEM_H

#include <QGraphicsLineItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"
#include "core/OntologyDataController.h".h"

class NodeItem;

class RelationItem : public QGraphicsLineItem, public OntologyGraphElement {
  private:
    NodeItem *m_sourceNode;
    NodeItem *m_destinationNode;

    OntologyDataController *m_dataController;

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

    void setRelatedDataController(OntologyDataController *dataController);
};

#endif // RELATIONITEM_H
