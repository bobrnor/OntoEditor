#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsRectItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"

class RelationItem;

class NodeItem : public QGraphicsRectItem, public OntologyGraphElement {
  private:
    QList<RelationItem *> m_relations;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  public:
    explicit NodeItem(QGraphicsItem *parent = 0);
    ~NodeItem();

    void appendRelation(RelationItem *relation);
    void removeRelation(RelationItem *relation);

    void removeAllRelations();

    Json::Value jsonRepresentation() const;
};

#endif // NODEITEM_H
