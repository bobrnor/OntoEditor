#include "RelationItem.h"

#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QFont>
#include <QFontMetrics>

#include <math.h>

#include "NodeItem.h"

RelationItem::RelationItem(QGraphicsItem *parent) :
  QGraphicsLineItem(parent), OntologyGraphElement(), m_sourceNode(NULL), m_destinationNode(NULL) {

  setPen(QPen(Qt::black, 1));
  setZValue(-1);

  setFlag(ItemIsSelectable);

  setData(kIDTType, kITRelation);
}

RelationItem::RelationItem(const Json::Value &jsonValue) :
  QGraphicsLineItem(NULL), OntologyGraphElement(jsonValue), m_sourceNode(NULL), m_destinationNode(NULL) {

  setPen(QPen(Qt::black, 1));
  setZValue(-1);

  setFlag(ItemIsSelectable);

  setData(kIDTType, kITRelation);
}

RelationItem::~RelationItem() {

  qDebug() << "Relation destructor";
}

QRectF RelationItem::boundingRect() const {

  QRectF boundingRect = QGraphicsLineItem::boundingRect();

  QFontMetrics metrics = QFontMetrics(QFont());
  QRect textRect = metrics.boundingRect(m_name);

  boundingRect.setX(boundingRect.x() - 10.0);
  boundingRect.setY(boundingRect.y() - 10.0);
  boundingRect.setWidth(boundingRect.width() + 10.0);
  boundingRect.setHeight(boundingRect.height() + 10.0);

  if (boundingRect.width() / 2 < textRect.width()) {
    boundingRect.setWidth(textRect.width() * 2);
  }

  if (boundingRect.height() / 2 < textRect.height()) {
    boundingRect.setHeight(textRect.height() * 2);
  }

  return boundingRect;
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
  QPointF middlePos = (sourcePos + destinationPos) / 2.0;

  double angle = ::acos(line().dx() / line().length());
  if (line().dy() >= 0)
    angle = (Pi * 2) - angle;
  QPointF arrowP1 = middlePos - QPointF(sin(angle + Pi / 3) * 10,
                                          cos(angle + Pi / 3) * 10);
  QPointF arrowP2 = middlePos - QPointF(sin(angle + Pi - Pi / 3) * 10,
                                          cos(angle + Pi - Pi / 3) * 10);

  QPainterPath path;
  path.moveTo(sourcePos);
  path.lineTo(destinationPos);
  path.moveTo(middlePos);
  path.lineTo(arrowP1);
  path.moveTo(middlePos);
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
  painter->drawText(middlePos, m_name);
}

Json::Value RelationItem::jsonRepresentation() const {

  Json::Value value = OntologyGraphElement::jsonRepresentation();
  value["source_node"] = (m_sourceNode != NULL)
                         ? Json::Value((Json::Int64)m_sourceNode->id())
                         : Json::Value(Json::nullValue);

  value["destination_node"] = (m_destinationNode != NULL)
                              ? Json::Value((Json::Int64)m_destinationNode->id())
                              : Json::Value(Json::nullValue);
  return value;
}
