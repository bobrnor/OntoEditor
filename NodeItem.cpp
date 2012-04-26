#include "NodeItem.h"

#include "RelationItem.h"

NodeItem::NodeItem(QGraphicsItem *parent) :
  QGraphicsRectItem(parent, NULL) {

  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemSendsGeometryChanges);
}

void NodeItem::appendRelation(RelationItem *relation) {

  if (!m_relations.contains(relation)) {
    m_relations.append(relation);
  }
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {

  if (change == ItemPositionHasChanged) {
    foreach (RelationItem *relation, m_relations) {
      relation->adjust();
    }
  }

  return QGraphicsRectItem::itemChange(change, value);
}
