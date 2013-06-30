#include "NodeItem.h"

#include <QDebug>
#include <QBrush>
#include <QPainter>
#include <QFontMetrics>

#include "RelationItem.h"

NodeItem::NodeItem(QGraphicsItem *parent) :
  QGraphicsRectItem(parent, NULL), OntologyGraphElement() {

  m_backgroundColor = Qt::white;
  m_textColor = Qt::black;
  m_shapeName = "rect";

  setRect(QRectF());

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

void NodeItem::setName(const QString &name) {

  OntologyGraphElement::setName(name);
  setRect(QRectF());
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {

  if (change == ItemPositionHasChanged) {
    foreach (RelationItem *relation, m_relations) {
      relation->adjust();
    }

    emit nodeItemPositionChangedSignal(this->id(), value.toPointF());
  }

  return QGraphicsRectItem::itemChange(change, value);
}

void NodeItem::attributesChanged() {

  NodeData *data = relatedDataController()->getNodeById(m_id);
  relatedDataController()->nodeAttributesChanged(m_id, data->attributes);

  if (data->attributes.keys().contains("gui-attributes")) {
    QVariantMap guiAttributes = data->attributes.value("gui-attributes").toMap();

    if (guiAttributes.contains("text_color")) {
      QString textColorHex = guiAttributes["text_color"].toString();
      m_textColor = QColor(textColorHex);
    }

    if (guiAttributes.contains("bg_color")) {
      QString bgColorHex = guiAttributes["bg_color"].toString();
      m_backgroundColor = QColor(bgColorHex);
    }

    if (guiAttributes.contains("shape_name")) {
      m_shapeName = guiAttributes["shape_name"].toString();
    }
  }

  if (this->scene() != NULL) {
    this->scene()->invalidate();
  }
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  if (this->rect().isNull()) {
    if (m_name.length() > 0) {
      QFontMetrics metrics = painter->fontMetrics();
      setRect(metrics.boundingRect(m_name));
    }
    else {
      setRect(QRectF(-60.0, -35.0, 120.0, 70.0));
    }
  }

  QPen shapePen = this->pen();
  QPen textPen = this->pen();
  QBrush shapeBrush = this->brush();

  if (isSelected()) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    shapePen.setDashPattern(dashPattern);
    shapePen.setColor(Qt::blue);
    textPen.setColor(Qt::blue);
    shapeBrush.setColor(m_backgroundColor);
  }
  else if (m_dataController->isNodeChanged(m_id)) {
    QVector<qreal> dashPattern;
    dashPattern.append(2.0);
    dashPattern.append(2.0);
    shapePen.setDashPattern(dashPattern);
    shapePen.setColor(Qt::red);
    textPen.setColor(Qt::red);
    shapeBrush.setColor(m_backgroundColor);
  }
  else {
    shapeBrush.setColor(m_backgroundColor);
    textPen.setColor(m_textColor);
  }

  QTextOption textOption;
  textOption.setAlignment(Qt::AlignCenter);
  textOption.setWrapMode(QTextOption::WordWrap);
  painter->setPen(shapePen);
  painter->setBrush(shapeBrush);

  if (m_shapeName == "rect") {
    painter->drawRect(this->rect());
  }
  else if (m_shapeName == "ellipse") {
    painter->drawEllipse(this->rect());
  }
  else if (m_shapeName == "rounded_rect") {
    painter->drawRoundedRect(this->rect(), 5.0, 5.0);
  }
  else {
    painter->drawRect(this->rect());
  }

  painter->setPen(textPen);  
  painter->drawText(boundingRect(), m_name, textOption);
}

QString NodeItem::attributesAsText() const {

  NodeData *nodeData = relatedDataController()->getNodeById(m_id);
  return nodeData->attributesAsText();
}

QVariantMap NodeItem::attributes() const {

  NodeData *nodeData = relatedDataController()->getNodeById(m_id);
  return nodeData->attributes;
}

void NodeItem::setAttributesFromData(const QByteArray &data) {

  NodeData *nodeData = relatedDataController()->getNodeById(m_id);
  nodeData->setAttributesFromData(data);
  attributesChanged();
}
