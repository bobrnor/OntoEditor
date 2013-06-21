#include "NodeItem.h"

#include <QDebug>
#include <QBrush>
#include <QPainter>

#include "RelationItem.h"

NodeItem::NodeItem(QGraphicsItem *parent) :
  QGraphicsRectItem(parent, NULL), OntologyGraphElement() {

  m_backgroundColor = Qt::white;
  m_textColor = Qt::black;
  m_shapeName = "rect";

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

    emit nodeItemPositionChangedSignal(this->id(), value.toPointF());
  }

  return QGraphicsRectItem::itemChange(change, value);
}

void NodeItem::attributesChanged() {

  NodeData *data = relatedDataController()->getNodeById(m_id);
  relatedDataController()->nodeAttributesChanged(m_id, data->attributes);

  if (data->attributes.keys().contains("text_color")) {
    QString textColorHex = data->attributes.value("text_color");
    m_textColor = QColor(textColorHex);
  }

  if (data->attributes.keys().contains("bg_color")) {
    QString bgColorHex = data->attributes.value("bg_color");
    m_backgroundColor = QColor(bgColorHex);
  }

  if (data->attributes.keys().contains("shape_name")) {
    m_shapeName = data->attributes.value("shape_name");
  }

  if (this->scene() != NULL) {
    this->scene()->invalidate();
  }
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

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

  NodeData *data = relatedDataController()->getNodeById(m_id);
  if (data->attributes.size() > 0) {
    Json::Value jsonValue;
    foreach (QString key, data->attributes.keys()) {
      QString value = data->attributes.value(key);
      jsonValue[key.toStdString()] = Json::Value(value.toStdString());
    }

    return QString::fromStdString(jsonValue.toStyledString());
  }
  else {
    return QString();
  }
}

QMap<QString, QString> NodeItem::attributest() const {

  NodeData *data = relatedDataController()->getNodeById(m_id);
  return data->attributes;
}

void NodeItem::setAttributes(const QString &text) {

  qDebug() << text;

  Json::Reader reader;
  Json::Value jsonValue;
  bool ok = reader.parse(text.toStdString(), jsonValue);

  if (ok) {
    NodeData *data = relatedDataController()->getNodeById(m_id);
    data->attributes.clear();

    for (int i = 0; i < jsonValue.size(); ++i) {
      QString key = QString::fromStdString(jsonValue.getMemberNames().at(i));
      QString value = QString::fromStdString(jsonValue[jsonValue.getMemberNames().at(i)].asString());
      data->attributes.insert(key, value);
    }

    attributesChanged();
  }
}
