#include "RelationItem.h"

#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "NodeItem.h"

RelationItem::RelationItem(QGraphicsItem *parent) :
  QGraphicsLineItem(parent), m_sourceNode(NULL), m_destinationNode(NULL) {

  setPen(QPen(Qt::black, 2));
  setZValue(-1);

  setFlag(ItemIsSelectable);

  setData(kIDTType, kITRelation);
}

void RelationItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  if (event->button() != Qt::LeftButton) {
    event->accept();
    return;
  }
  QGraphicsLineItem::mousePressEvent(event);
}

void RelationItem::setSourceNode(NodeItem *node) {

  m_sourceNode = node;
  if (m_sourceNode != NULL) {
    m_sourceNode->appendRelation(this);
  }
  adjust();
}

NodeItem *RelationItem::sourceNode() const {

  return m_sourceNode;
}

void RelationItem::setDestinationNode(NodeItem *node) {

  m_destinationNode = node;
  if (m_destinationNode != NULL) {
    m_destinationNode->appendRelation(this);
  }
  adjust();
}

NodeItem *RelationItem::destinationNode() const {

  return m_destinationNode;
}

void RelationItem::removeFromNodes() {

  if (m_sourceNode != NULL) {
    m_sourceNode->removeRelation(this);
    m_sourceNode = NULL;
  }

  if (m_destinationNode != NULL) {
    m_destinationNode->removeRelation(this);
    m_destinationNode = NULL;
  }
}

void RelationItem::adjust() {

  if (m_sourceNode != NULL && m_destinationNode != NULL) {
    setLine(QLineF(m_sourceNode->pos(), m_destinationNode->pos()));
  }
}

void RelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  QPen pen = this->pen();

  if (isSelected()) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    pen.setDashPattern(dashPattern);
    pen.setColor(Qt::blue);
  }

  painter->setPen(pen);
  painter->drawLine(line());
}
