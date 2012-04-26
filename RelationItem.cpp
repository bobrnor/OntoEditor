#include "RelationItem.h"

#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <math.h>

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

  qreal Pi = 3.14;
  QPointF sourcePos(line().p1());
  QPointF destinationPos(line().p2());
  double angle = ::acos(line().dx() / line().length());
  if (line().dy() >= 0)
    angle = (Pi * 2) - angle;
  QPointF arrowP1 = line().p2() - QPointF(sin(angle + Pi / 3) * 10,
                                          cos(angle + Pi / 3) * 10);
  QPointF arrowP2 = line().p2() - QPointF(sin(angle + Pi - Pi / 3) * 10,
                                          cos(angle + Pi - Pi / 3) * 10);

  QPainterPath path;
  path.moveTo(sourcePos);
  path.lineTo(destinationPos);
  path.lineTo(arrowP1);
  path.moveTo(destinationPos);
  path.lineTo(arrowP2);

  QPen pen = this->pen();

  if (isSelected()) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    pen.setDashPattern(dashPattern);
    pen.setColor(Qt::blue);
  }

  painter->setPen(pen);
  painter->drawPath(path);
}
