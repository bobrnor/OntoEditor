#include "RelationItem.h"

#include <QPen>
#include <QPainter>
#include "NodeItem.h"

RelationItem::RelationItem(QGraphicsItem *parent) :
  QGraphicsLineItem(parent), m_sourceNode(NULL), m_destinationNode(NULL) {

  setPen(QPen(Qt::black, 2));
  setZValue(-1);

//  setFlag(ItemIsSelectable);
}

void RelationItem::setSourceNode(NodeItem *node) {

  m_sourceNode = node;
  if (m_sourceNode != NULL) {
    m_sourceNode->appendRelation(this);
  }
  adjust();
}

NodeItem *RelationItem::getSourceNode() const {

  return m_sourceNode;
}

void RelationItem::setDestinationNode(NodeItem *node) {

  m_destinationNode = node;
  if (m_destinationNode != NULL) {
    m_destinationNode->appendRelation(this);
  }
  adjust();
}

NodeItem *RelationItem::getDestinationNode() const {

  return m_destinationNode;
}

void RelationItem::adjust() {

    if (m_sourceNode != NULL && m_destinationNode != NULL) {
        setLine(QLineF(m_sourceNode->pos(), m_destinationNode->pos()));
    }
}

/*void RelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  QPen pen;

  if (isSelected()) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    pen.setDashPattern(dashPattern);
    pen.setColor(Qt::blue);
  }
  else {
    pen.setColor(Qt::black);
  }

  painter->setPen(pen);
  painter->drawLine(line());
}*/
