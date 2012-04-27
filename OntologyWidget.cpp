#include "OntologyWidget.h"
#include "ui_OntologyWidget.h"

#include <QMenu>
#include <QDebug>
#include <QInputDialog>

#include "lib_json/json/json.h"

#include "NodeItem.h"
#include "RelationItem.h"
#include "RelationVisualizedLine.h"

OntologyWidget::OntologyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::OntologyWidget) {

  ui->setupUi(this);

  m_ontologyView = new OntologyGraphicsView(this);
  m_ontologyView->setDragMode(QGraphicsView::RubberBandDrag);
  m_ontologyView->setContextMenuPolicy(Qt::CustomContextMenu);
  m_ontologyView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  QGraphicsScene *scene = new QGraphicsScene(m_ontologyView);
  QBrush bgBrush = QBrush(Qt::Dense7Pattern);
  bgBrush.setColor(Qt::lightGray);
  scene->setBackgroundBrush(bgBrush);

  m_ontologyView->setScene(scene);

  connect(m_ontologyView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenuSlot(QPoint)));

  m_delegate = NULL;
  m_relationVisualizedLine = NULL;
  m_editRelationMode = false;

  ui->verticalLayout->addWidget(m_ontologyView);
}

OntologyWidget::~OntologyWidget() {

  delete ui;
}

void OntologyWidget::setDelegate(IOntologyWidgetDelegate *delegate) {

  m_delegate = delegate;
}

IOntologyWidgetDelegate *OntologyWidget::delegate() const {

  return m_delegate;
}

void OntologyWidget::showContextMenuSlot(const QPoint &pos) {

  serialize();

  QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();

  QMenu contextMenu;
  QAction *addNodeAction = contextMenu.addAction(tr("Add node..."));

  if (selectedItems.count() == 1) {
    QGraphicsItem *selectedItem = selectedItems.at(0);
    if (selectedItem->data(kIDTType).toInt() == kITNode) {
      QAction *addRelationAction = contextMenu.addAction(tr("Set relation with..."));
      connect(addRelationAction, SIGNAL(triggered()), SLOT(setRelationSlot()));
    }
  }
  contextMenu.addSeparator();

  connect(addNodeAction, SIGNAL(triggered()), SLOT(addNodeSlot()));

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

  m_lastRightClickScenePosition = m_ontologyView->mapToScene(pos);
  QPoint globalPos = m_ontologyView->mapToGlobal(pos);
  contextMenu.exec(globalPos);
}

void OntologyWidget::setEditRelationMode(bool on) {

  if (on) {
    m_ontologyView->scene()->clearSelection();
    m_editRelationMode = true;
    m_ontologyView->setMouseTracking(true);
    m_ontologyView->setDragMode(QGraphicsView::NoDrag);
    connect(m_ontologyView, SIGNAL(mousePositionChangedSignal(QPoint)), SLOT(ontologyViewMousePositionChangedSlot(QPoint)));
    connect(m_ontologyView->scene(), SIGNAL(selectionChanged()), SLOT(sceneSelectionChangedSlot()));
  }
  else {
    m_editRelationMode = false;
    m_ontologyView->setMouseTracking(false);
    m_ontologyView->setDragMode(QGraphicsView::RubberBandDrag);
    disconnect(this, SLOT(ontologyViewMousePositionChangedSlot(QPoint)));

    if (m_relationVisualizedLine != NULL) {
      m_relationVisualizedLine->removeFromNodes();
      m_ontologyView->scene()->removeItem(m_relationVisualizedLine);
      delete m_relationVisualizedLine;
      m_relationVisualizedLine = NULL;
    }
  }
}

void OntologyWidget::addNodeSlot() {

  QPointF scenePos = m_lastRightClickScenePosition;
  QRectF sceneRect(QPointF(-75, -25), QSizeF(150, 50));

  long newNodeId = -1;
  if (m_delegate != NULL) {
    newNodeId = m_delegate->nodeCreated();
  }

  NodeItem *newNode = new NodeItem(NULL);
  newNode->setId(newNodeId);
  newNode->setPos(scenePos);
  newNode->setRect(sceneRect);
  m_ontologyView->scene()->addItem(newNode);
}

void OntologyWidget::setRelation(NodeItem *sourceNode, NodeItem *destinationNode) {

  long newRelationId = -1;
  if (m_delegate != NULL) {
    newRelationId = m_delegate->relatoinCreated(sourceNode->id(), destinationNode->id());
  }

  RelationItem *relationItem = new RelationItem();
  relationItem->setId(newRelationId);
  relationItem->setSourceNode(sourceNode);
  relationItem->setDestinationNode(destinationNode);
  m_ontologyView->scene()->addItem(relationItem);
}

void OntologyWidget::validateItems() {

  QList<QGraphicsItem *> items = m_ontologyView->scene()->items();
  foreach (QGraphicsItem *item, items) {
    if (item->data(kIDTType) == kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(item);
      if (relationItem->sourceNode() == NULL && relationItem->destinationNode() == NULL) {
        m_ontologyView->scene()->removeItem(relationItem);

        if (m_delegate != NULL) {
          m_delegate->relatoinRemoved(relationItem->id());
        }

        delete relationItem;
      }
    }
  }
}

void OntologyWidget::setRelationSlot() {

  QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();
  if (selectedItems.count() == 1) {
    QGraphicsItem *selectedItem = selectedItems.at(0);
    if (selectedItem->data(kIDTType).toInt() == kITNode) {
      NodeItem *sourceNodeItem = static_cast<NodeItem *>(selectedItem);

      if (m_relationVisualizedLine != NULL) {
        m_relationVisualizedLine->removeFromNodes();
        m_ontologyView->scene()->removeItem(m_relationVisualizedLine);
        delete m_relationVisualizedLine;
        m_relationVisualizedLine = NULL;
      }

      m_relationVisualizedLine = new RelationVisualizedLine();
      m_relationVisualizedLine->setSourceNode(sourceNodeItem);
      m_ontologyView->scene()->addItem(m_relationVisualizedLine);

      setEditRelationMode(true);
    }
  }
}

void OntologyWidget::editNodeSlot() {

  QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();
  if (selectedItems.count() == 1) {
    QGraphicsItem *selectedItem = selectedItems.at(0);
    if (selectedItem->data(kIDTType).toInt() == kITNode) {
      NodeItem *nodeItem = static_cast<NodeItem *>(selectedItem);

      bool ok = false;
      QString newName = QInputDialog::getText(this,
                                              tr("Enter new node name"),
                                              tr("Name: "),
                                              QLineEdit::Normal,
                                              nodeItem->name(),
                                              &ok);
      if (ok) {
        if (m_delegate != NULL) {
          m_delegate->nodeNameChanged(nodeItem->id(), newName);
        }
        nodeItem->setName(newName);
        m_ontologyView->scene()->invalidate();
      }
    }
  }
}

void OntologyWidget::editRelationSlot() {

  QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();
  if (selectedItems.count() == 1) {
    QGraphicsItem *selectedItem = selectedItems.at(0);
    if (selectedItem->data(kIDTType).toInt() == kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(selectedItem);

      bool ok = false;
      QString newName = QInputDialog::getText(this,
                                              tr("Enter new node name"),
                                              tr("Name: "),
                                              QLineEdit::Normal,
                                              relationItem->name(),
                                              &ok);
      if (ok) {
        if (m_delegate != NULL) {
          m_delegate->relationNameChanged(relationItem->id(), newName);
        }
        relationItem->setName(newName);
        m_ontologyView->scene()->invalidate();
      }
    }
  }
}

void OntologyWidget::removeSelectedSlot() {

  QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();
  foreach (QGraphicsItem *item, selectedItems) {
    if (item->data(kIDTType) == kITNode) {
      NodeItem *nodeItem = static_cast<NodeItem *>(item);
      nodeItem->removeAllRelations();
      if (m_delegate != NULL) {
        m_delegate->nodeRemoved(nodeItem->id());
      }
    }
    else if (item->data(kIDTType) == kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(item);
      relationItem->removeFromNodes();
      if (m_delegate != NULL) {
        m_delegate->relatoinRemoved(relationItem->id());
      }
    }
    m_ontologyView->scene()->removeItem(item);
    delete item;
  }
  validateItems();
}

void OntologyWidget::sceneSelectionChangedSlot() {

  if (m_editRelationMode) {
    QList<QGraphicsItem *> selectedItems = m_ontologyView->scene()->selectedItems();
    if (selectedItems.count() == 1) {
      QGraphicsItem *selectedItem = selectedItems.at(0);
      if (selectedItem->data(kIDTType) == kITNode) {
        setRelation(m_relationVisualizedLine->sourceNode(), static_cast<NodeItem *>(selectedItem));
        setEditRelationMode(false);
      }
      else {
        m_ontologyView->scene()->clearSelection();
      }
    }
  }
}

void OntologyWidget::ontologyViewMousePositionChangedSlot(const QPoint &pos) {

  if (m_relationVisualizedLine != NULL) {
    QPointF scenePos = m_ontologyView->mapToScene(pos);
    m_relationVisualizedLine->setEndPoint(scenePos);
  }
}

Json::Value OntologyWidget::serialize() const {

  Json::Value value;
  Json::Value itemsJson(Json::arrayValue);
  foreach (QGraphicsItem *item, m_ontologyView->scene()->items()) {
    if (item->data(kIDTType) != kITNode) {
      NodeItem *nodeItem = static_cast<NodeItem *>(item);
      itemsJson.append(nodeItem->jsonRepresentation());
    }
    else if (item->data(kIDTType) != kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(item);
      itemsJson.append(relationItem->jsonRepresentation());
    }
  }
  value["items"] = itemsJson;

  qDebug() << QString::fromStdString(value.toStyledString());

  return value;
}
void OntologyWidget::deserialize(const Json::Value &json) {

}

