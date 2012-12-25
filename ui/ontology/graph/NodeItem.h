#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsRectItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"
#include "core/IOntologyDataSource.h"

class RelationItem;

class NodeItem : public QObject, public QGraphicsRectItem, public OntologyGraphElement {
    Q_OBJECT

  private:
    QList<RelationItem *> m_relations;

    IOntologyDataSource *m_dataSource;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  public:
    explicit NodeItem(QGraphicsItem *parent = 0);
    ~NodeItem();

    void appendRelation(RelationItem *relation);
    void removeRelation(RelationItem *relation);

    void removeAllRelations();

    void setRelatedDataSource(IOntologyDataSource *dataSource);

    Json::Value jsonRepresentation() const;

  signals:
    void nodeItemPositionChangedSignal(long id, const QPointF &newPosition);
};

#endif // NODEITEM_H
