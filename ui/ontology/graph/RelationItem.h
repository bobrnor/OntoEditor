#ifndef RELATIONITEM_H
#define RELATIONITEM_H

#include <QGraphicsLineItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"

class NodeItem;

class RelationItem : public QGraphicsLineItem, public OntologyGraphElement {
  private:
    NodeItem *m_sourceNode;
    NodeItem *m_destinationNode;

    double m_width;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void attributesChanged();

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

    QString attributesAsText() const;
    Json::Value attributesAsJson() const;
    QMap<QString, QString> attributes() const;
    void setAttributes(const QString &text);
};

#endif // RELATIONITEM_H
