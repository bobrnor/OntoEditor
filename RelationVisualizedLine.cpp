#include "RelationVisualizedLine.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

#include "NodeItem.h"

RelationVisualizedLine::RelationVisualizedLine(QGraphicsItem *parent) :
  RelationItem(parent), m_sourceNode(NULL) {

  setPen(QPen(Qt::black, 2));
  m_endPointEmpty = true;
}

void RelationVisualizedLine::setSourceNode(NodeItem *node) {

  m_sourceNode = node;
  if (m_sourceNode != NULL) {
    m_sourceNode->appendRelation(this);
  }
  adjust();
}

NodeItem *RelationVisualizedLine::sourceNode() const {

  return m_sourceNode;
}

void RelationVisualizedLine::setEndPoint(QPointF point) {

  m_endPoint = point;
  m_endPointEmpty = false;
  adjust();
}

QPointF RelationVisualizedLine::endPoint() const {

  return m_endPoint;
}

void RelationVisualizedLine::adjust() {

  if (m_sourceNode != NULL && !m_endPointEmpty) {
    setLine(QLineF(m_sourceNode->scenePos(), m_endPoint));
  }
}

void RelationVisualizedLine::removeFromNodes() {

  if (m_sourceNode != NULL) {
    m_sourceNode->removeRelation(this);
  }
}

void RelationVisualizedLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  QPen pen = this->pen();

  QVector<qreal> dashPattern;
  dashPattern.append(2.0);
  dashPattern.append(2.0);
  pen.setDashPattern(dashPattern);
  pen.setColor(Qt::lightGray);

  painter->setPen(pen);
  qDebug() << line();
  painter->drawLine(line());
}
