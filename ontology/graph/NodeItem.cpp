#include "NodeItem.h"

#include <QDebug>
#include <QBrush>
#include <QPainter>

#include "RelationItem.h"

NodeItem::NodeItem(QGraphicsItem *parent) :
  QGraphicsRectItem(parent, NULL), OntologyGraphElement() {

  QRectF rect(QPointF(-75, -40), QSizeF(150, 80));
  setRect(rect);

  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemSendsGeometryChanges);

  QBrush brush = QBrush(Qt::SolidPattern);
  brush.setColor(Qt::white);
  setBrush(brush);

  setData(kIDTType, kITNode);
}

NodeItem::~NodeItem() {

  qDebug() << "NodeItem destructor";
}

void NodeItem::appendRelation(RelationItem *relation) {

  if (!m_relations.contains(relation)) {
    m_relations.append(relation);
  }
}

void NodeItem::removeRelation(RelationItem *relation) {

  m_relations.removeAll(relation);
}

void NodeItem::removeAllRelations() {

  foreach (RelationItem *item, m_relations) {
    item->removeFromNodes();
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

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  QGraphicsRectItem::paint(painter, option, widget);

  QTextOption textOption;
  textOption.setAlignment(Qt::AlignCenter);
  textOption.setWrapMode(QTextOption::WordWrap);
  painter->drawText(boundingRect(), m_name, textOption);
}

Json::Value NodeItem::jsonRepresentation() const {

  Json::Value value = OntologyGraphElement::jsonRepresentation();
  value["pos_x"] = Json::Value(pos().x());
  value["pos_y"] = Json::Value(pos().y());
  return value;
}
