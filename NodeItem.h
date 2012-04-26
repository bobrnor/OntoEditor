#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsRectItem>

class RelationItem;

class NodeItem : public QGraphicsRectItem {
  private:
    QList<RelationItem *> m_relations;

  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  public:
    explicit NodeItem(QGraphicsItem *parent = 0);

    void appendRelation(RelationItem *relation);
    
  signals:
    
  public slots:
    
};

#endif // NODEITEM_H
