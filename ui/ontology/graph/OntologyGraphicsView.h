#ifndef ONTOLOGYGRAPHICSVIEW_H
#define ONTOLOGYGRAPHICSVIEW_H

#include <QMouseEvent>
#include <QGraphicsView>

class OntologyGraphicsView : public QGraphicsView {
    Q_OBJECT

  protected:
    void mouseMoveEvent(QMouseEvent *event);

//    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

  public:
    explicit OntologyGraphicsView(QWidget *parent = 0);
    explicit OntologyGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);

  signals:
    void mousePositionChangedSignal(const QPoint &pos);
};

#endif // ONTOLOGYGRAPHICSVIEW_H
