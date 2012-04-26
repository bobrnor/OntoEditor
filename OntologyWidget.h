#ifndef ONTOLOGYWIDGET_H
#define ONTOLOGYWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>

namespace Ui {
  class OntologyWidget;
}

class OntologyWidget : public QWidget {
    Q_OBJECT
    
  public:
    explicit OntologyWidget(QWidget *parent = 0);
    ~OntologyWidget();
    
  private:
    Ui::OntologyWidget *ui;

    QPointF m_lastRightClickScenePosition;

  public slots:
    void showContextMenuSlot(const QPoint &pos);

    void addNodeSlot();
    void addRelationSlot();
    void editNodeSlot();
    void editRelationSlot();
    void removeSelectedSlot();
};

#endif // ONTOLOGYWIDGET_H
