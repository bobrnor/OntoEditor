#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsRectItem>

#include "ItemDataKey.h"
#include "ItemType.h"
#include "OntologyGraphElement.h"
#include "core/OntologyDataController.h"

class RelationItem;

class NodeItem : public QObject, public QGraphicsRectItem, public OntologyGraphElement {
    Q_OBJECT

  private:
    QList<RelationItem *> m_relations;

    OntologyDataController *m_dataController;

  protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  public:
    explicit NodeItem(QGraphicsItem *parent = 0);
    ~NodeItem();

    void appendRelation(RelationItem *relation);
    void removeRelation(RelationItem *relation);

    void removeAllRelations();

    void setRelatedDataControlelr(OntologyDataController *dataController);

    Json::Value jsonRepresentation() const;

  signals:
    void nodeItemPositionChangedSignal(long id, const QPointF &newPosition);
};

#endif // NODEITEM_H
