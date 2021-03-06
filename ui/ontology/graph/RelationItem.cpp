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
  m_width = 1.0;

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

  if (this->scene() != NULL) {
    this->scene()->invalidate();
  }
}

void RelationItem::attributesChanged() {

  RelationData *data = relatedDataController()->getRelationById(m_id);
  relatedDataController()->relationAttributesChanged(m_id, data->attributes);

  m_dashPattern.clear();

  if (data->attributes.keys().contains("gui-attributes")) {
    QVariantMap guiAttributes = data->attributes.value("gui-attributes").toMap();

    if (guiAttributes.contains("line_width")) {
      QString lineWidth = guiAttributes["line_width"].toString();
      m_width = lineWidth.toDouble();
    }

    if (guiAttributes.contains("dash")) {
      QString dash = guiAttributes["dash"].toString();
      if (dash == "solid") {
//        m_dashPattern.append(1.0);
      }
      else if (dash == "dash") {
        m_dashPattern.append(3.0);
        m_dashPattern.append(2.0);
      }
      else if (dash == "dot") {
        m_dashPattern.append(1.0);
        m_dashPattern.append(2.0);
      }
    }
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
  pen.setWidthF(m_width);
  pen.setDashPattern(m_dashPattern);

  if (isSelected()) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    pen.setDashPattern(dashPattern);
    pen.setColor(Qt::blue);
  }
  else if (m_dataController->isRelationChanged(m_id)) {
//    QVector<qreal> dashPattern;
//    dashPattern.append(2.0);
//    dashPattern.append(2.0);
//    pen.setDashPattern(dashPattern);
    pen.setColor(Qt::red);
  }

  painter->setPen(pen);
  painter->drawPath(path);
  painter->drawText(middlePos, m_name);
}

QString RelationItem::attributesAsText() const {

  RelationData *relationData = relatedDataController()->getRelationById(m_id);
  return relationData->attributesAsText();
}

QVariantMap RelationItem::attributes() const {

  RelationData *relationData = relatedDataController()->getRelationById(m_id);
  return relationData->attributes;
}

void RelationItem::setAttributesFromData(const QByteArray &data) {

  RelationData *relationData = relatedDataController()->getRelationById(m_id);
  relationData->setAttributesFromData(data);
  attributesChanged();
}

void RelationItem::setAttributes(const QVariantMap &attributes) {

  RelationData *relationData = relatedDataController()->getRelationById(m_id);
  relationData->attributes = attributes;
  attributesChanged();
}
