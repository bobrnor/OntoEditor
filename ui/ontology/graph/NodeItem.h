#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsRectItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"

class RelationItem;

class NodeItem : public QObject, public QGraphicsRectItem, public OntologyGraphElement {
    Q_OBJECT

  private:
    QList<RelationItem *> m_relations;
    QColor m_backgroundColor;
    QColor m_textColor;
    QString m_shapeName;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void attributesChanged();

  public:
    explicit NodeItem(QGraphicsItem *parent = 0);
    ~NodeItem();

    void appendRelation(RelationItem *relation);
    void removeRelation(RelationItem *relation);

    void removeAllRelations();

    QString attributesAsText() const;
    Json::Value attributesAsJson() const;
    QMap<QString, QString> attributes() const;
    void setAttributes(const QString &text);

  signals:
    void nodeItemPositionChangedSignal(long id, const QPointF &newPosition);
};

#endif // NODEITEM_H
