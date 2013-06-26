#include "OntologyGraphicsView.h"

OntologyGraphicsView::OntologyGraphicsView(QWidget *parent)
  : QGraphicsView(parent) {

}

OntologyGraphicsView::OntologyGraphicsView(QGraphicsScene *scene, QWidget *parent)
  : QGraphicsView(scene, parent) {

}

void OntologyGraphicsView::mouseMoveEvent(QMouseEvent *event) {

  if (hasMouseTracking()) {
    emit mousePositionChangedSignal(event->pos());
  }
  QGraphicsView::mouseMoveEvent(event);
}

void OntologyGraphicsView::dragEnterEvent(QDragEnterEvent *event) {

}
