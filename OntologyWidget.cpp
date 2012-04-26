#include "OntologyWidget.h"
#include "ui_OntologyWidget.h"

#include <QMenu>
#include <QDebug>

#include "NodeItem.h"
#include "RelationItem.h"

OntologyWidget::OntologyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::OntologyWidget) {

  ui->setupUi(this);

  ui->ontologyView->setDragMode(QGraphicsView::RubberBandDrag);

  QGraphicsView *graphicsView = ui->ontologyView;
  QGraphicsScene *scene = new QGraphicsScene(graphicsView);
//  scene->setSceneRect(0, 0, 800, 600);

  QBrush bgBrush = QBrush(Qt::Dense7Pattern);
  bgBrush.setColor(Qt::lightGray);

  scene->setBackgroundBrush(bgBrush);

  graphicsView->setScene(scene);

  graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(graphicsView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenuSlot(QPoint)));
}

OntologyWidget::~OntologyWidget() {

  delete ui;
}

void OntologyWidget::showContextMenuSlot(const QPoint &pos) {

  QMenu contextMenu;
  QAction *addNodeAction = contextMenu.addAction(tr("Add node..."));
  QAction *addRelationAction = contextMenu.addAction(tr("Add relation..."));
  contextMenu.addSeparator();

  connect(addNodeAction, SIGNAL(triggered()), SLOT(addNodeSlot()));
  connect(addRelationAction, SIGNAL(triggered()), SLOT(addRelationSlot()));

  QList<QGraphicsItem *> selectedItems = ui->ontologyView->scene()->selectedItems();

  if (selectedItems.count() == 1) {
    QGraphicsItem *selectedItem = selectedItems.at(0);
    if (selectedItem->data(kIDTType).toInt() == kITNode) {
      QAction *editNodeAction = contextMenu.addAction(tr("Edit node..."));
      connect(editNodeAction, SIGNAL(triggered()), SLOT(editNodeSlot()));
    }
    else if (selectedItem->data(kIDTType).toInt() == kITRelation) {
      QAction *editRelationAction = contextMenu.addAction(tr("Edit relation..."));
      connect(editRelationAction, SIGNAL(triggered()), SLOT(editRelationSlot()));
    }
  }
  else {
    QAction *editAction = contextMenu.addAction(tr("Edit..."));
    editAction->setEnabled(false);

  }
  contextMenu.addSeparator();

  QAction *removeAction = contextMenu.addAction(tr("Remove..."));

  if (selectedItems.count() == 0) {
    removeAction->setEnabled(false);
  }
  else {
    connect(removeAction, SIGNAL(triggered()), SLOT(removeSelectedSlot()));
  }

  m_lastRightClickScenePosition = ui->ontologyView->mapToScene(pos);
  QPoint globalPos = ui->ontologyView->mapToGlobal(pos);
  contextMenu.exec(globalPos);
}

void OntologyWidget::addNodeSlot() {

  QPointF scenePos = m_lastRightClickScenePosition - QPointF(75, 25);
  QRectF sceneRect(scenePos, QSizeF(150, 50));

  NodeItem *newNode = new NodeItem(NULL);
  newNode->setRect(sceneRect);
  ui->ontologyView->scene()->addItem(newNode);
}

void OntologyWidget::addRelationSlot() {

}

void OntologyWidget::editNodeSlot() {

}

void OntologyWidget::editRelationSlot() {

}

void OntologyWidget::removeSelectedSlot() {

}
