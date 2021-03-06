#include "OntologyWidget.h"
#include "ui_OntologyWidget.h"
#include "ui_AttributeEditor.h"

#include <QMenu>
#include <QDebug>
#include <QInputDialog>

#include "NodeItem.h"
#include "RelationItem.h"
#include "RelationVisualizedLine.h"

OntologyWidget::OntologyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::OntologyWidget) {

  ui->setupUi(this);

  setAcceptDrops(true);

  m_ontologyView = new OntologyGraphicsView(this);
  m_ontologyView->setDragMode(QGraphicsView::RubberBandDrag);
  m_ontologyView->setContextMenuPolicy(Qt::CustomContextMenu);
  m_ontologyView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  m_sourceCodeViewer = new QTextEdit(this);
  m_sourceCodeViewer->setReadOnly(true);
  m_sourceCodeViewer->setVisible(false);

  QGraphicsScene *scene = new QGraphicsScene(m_ontologyView);
  QBrush bgBrush = QBrush(Qt::Dense7Pattern);
  bgBrush.setColor(Qt::lightGray);
  scene->setBackgroundBrush(bgBrush);

  m_ontologyView->setScene(scene);

  connect(m_ontologyView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenuSlot(QPoint)));

  m_dataController = NULL;
  m_relationVisualizedLine = NULL;
  m_editRelationMode = false;
  m_sourceCodeShown = false;

  ui->verticalLayout->addWidget(m_ontologyView);
}

OntologyWidget::~OntologyWidget() {

  delete ui;
}

void OntologyWidget::setDataController(OntologyDataController *dataController) {

  m_dataController = dataController;
}

OntologyDataController *OntologyWidget::dataController() const {

  return m_dataController;
}

void OntologyWidget::showContextMenuSlot(const QPoint &pos) {

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

      QMenu *dashMenu = contextMenu.addMenu("Dash");
      QAction *solidDashAction = dashMenu->addAction("Solid");
      QAction *dashDashAction = dashMenu->addAction("Dash");
      QAction *dotDashAction = dashMenu->addAction("Dot");

      connect(solidDashAction, SIGNAL(triggered()), SLOT(setSolidDashPatternSlot()));
      connect(dashDashAction, SIGNAL(triggered()), SLOT(setDashDashPatternSlot()));
      connect(dotDashAction, SIGNAL(triggered()), SLOT(setDotDashPatternSlot()));
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

  if (m_ontologyView->scene()->selectedItems().count() == 1) {
    contextMenu.addSeparator();
    QAction *editAttrsAction = contextMenu.addAction(tr("Edit attributes..."));
    connect(editAttrsAction, SIGNAL(triggered()), SLOT(editAttrsSlot()));
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

NodeItem *OntologyWidget::addNode(QPointF scenePos) {

  long newNodeId = -1;
  if (m_dataController != NULL) {
    newNodeId = m_dataController->nodeCreated();
    emit dataChangedSignal();
  }

  NodeItem *newNode = new NodeItem(NULL);
  newNode->setRelatedDataController(m_dataController);
  newNode->setId(newNodeId);
  newNode->setPos(scenePos);
  connect(newNode, SIGNAL(nodeItemPositionChangedSignal(long, QPointF)), SLOT(nodeItemPositionChangedSlot(long, QPointF)));
  m_ontologyView->scene()->addItem(newNode);
  return newNode;
}

void OntologyWidget::addNodeSlot() {

  QPointF scenePos = m_lastRightClickScenePosition;
  addNode(scenePos);
}

void OntologyWidget::setRelation(NodeItem *sourceNode, NodeItem *destinationNode) {

  long newRelationId = -1;
  if (m_dataController != NULL) {
    newRelationId = m_dataController->relationCreated(sourceNode->id(), destinationNode->id());
    emit dataChangedSignal();
  }

  RelationItem *relationItem = new RelationItem();
  relationItem->setRelatedDataController(m_dataController);
  relationItem->setId(newRelationId);
  relationItem->setSourceNode(sourceNode);
  relationItem->setDestinationNode(destinationNode);
  m_ontologyView->scene()->addItem(relationItem);
}

void OntologyWidget::updateData() {

  if (m_dataController == NULL) {
    this->setEnabled(false);
  }
  else {
    this->setEnabled(true);    

    QMap<long, NodeItem *> invalidatedNodesMap;
    QMap<long, RelationItem *> invalidatedRelationsMap;

    QList<QGraphicsItem *> items = m_ontologyView->scene()->items();
    foreach (QGraphicsItem *item, items) {
      if (item->data(kIDTType) == kITNode) {
        NodeItem *nodeItem = static_cast<NodeItem *>(item);
        invalidatedNodesMap.insert(nodeItem->id(), nodeItem);
      }
      else if (item->data(kIDTType) == kITRelation) {
        RelationItem *relationItem = static_cast<RelationItem *>(item);
        invalidatedRelationsMap.insert(relationItem->id(), relationItem);
      }
    }

    QMap<long, NodeItem *> existedNodes;
    int nodeCount = m_dataController->nodeCount();
    for (int i = 0; i < nodeCount; ++i) {
      NodeData *nodeData = m_dataController->getNodeByIndex(i);
      if (invalidatedNodesMap.contains(nodeData->id)) {
        NodeItem *nodeItem = invalidatedNodesMap.value(nodeData->id);
        nodeItem->setRelatedDataController(m_dataController);
        nodeItem->setName(nodeData->name);
        invalidatedNodesMap.remove(nodeItem->id());
        existedNodes.insert(nodeItem->id(), nodeItem);
      }
      else {
        QPointF pos = m_dataController->nodePosition(nodeData->id);

        NodeItem *nodeItem = new NodeItem();
        nodeItem->setRelatedDataController(m_dataController);
        nodeItem->setId(nodeData->id);
        nodeItem->setName(nodeData->name);
        nodeItem->setPos(pos);
        m_ontologyView->scene()->addItem(nodeItem);
        existedNodes.insert(nodeItem->id(), nodeItem);
        connect(nodeItem,
                SIGNAL(nodeItemPositionChangedSignal(long, QPointF)),
                SLOT(nodeItemPositionChangedSlot(long, QPointF)));
      }
    }

    int relationCount = m_dataController->relationCount();
    for (int i = 0; i < relationCount; ++i) {
      RelationData *relationData = m_dataController->getRelationByIndex(i);
      if (invalidatedRelationsMap.contains(relationData->id)) {
        RelationItem *relationItem = invalidatedRelationsMap.value(relationData->id);
        relationItem->setRelatedDataController(m_dataController);
        relationItem->setName(relationData->name);
        invalidatedRelationsMap.remove(relationItem->id());
      }
      else {
        RelationItem *relationItem = new RelationItem();
        relationItem->setRelatedDataController(m_dataController);
        relationItem->setId(relationData->id);
        relationItem->setName(relationData->name);

        NodeItem *sourceNode = existedNodes.value(relationData->sourceNodeId);
        NodeItem *destinationNode = existedNodes.value(relationData->destinationNodeId);

        relationItem->setSourceNode(sourceNode);
        relationItem->setDestinationNode(destinationNode);

        m_ontologyView->scene()->addItem(relationItem);
      }
    }

    foreach (NodeItem *invalidNode, invalidatedNodesMap.values()) {
      m_ontologyView->scene()->removeItem(invalidNode);
      delete invalidNode;
    }

    foreach (RelationItem *invalidRelation, invalidatedRelationsMap.values()) {
      m_ontologyView->scene()->removeItem(invalidRelation);
      delete invalidRelation;
    }
  }

  m_ontologyView->scene()->invalidate();
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
        if (m_dataController != NULL) {
          m_dataController->nodeNameChanged(nodeItem->id(), newName);
          emit dataChangedSignal();
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

      QStringList items;
      int relationsCount = m_dataController->relationCount();
      for (int i = 0; i < relationsCount; ++i) {
        RelationData *relation = m_dataController->getRelationByIndex(i);
        if (!items.contains(relation->name)) {
          items.append(relation->name);
        }
      }

      QString newName = QInputDialog::getItem(this,
                                              tr("Enter new node name"),
                                              tr("Name: "),
                                              items,
                                              0,
                                              true,
                                              &ok,
                                              0);

      if (ok) {
        if (m_dataController != NULL) {
          m_dataController->relationNameChanged(relationItem->id(), newName);
          emit dataChangedSignal();
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
      if (m_dataController != NULL) {
        m_dataController->nodeRemoved(nodeItem->id());
      }
    }
    else if (item->data(kIDTType) == kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(item);
      relationItem->removeFromNodes();
      if (m_dataController != NULL) {
        m_dataController->relationRemoved(relationItem->id());
      }
    }
    m_ontologyView->scene()->removeItem(item);
    delete item;
  }
  updateData();
  emit dataChangedSignal();
}

void OntologyWidget::editAttrsSlot() {

  QGraphicsItem *selectedItem = m_ontologyView->scene()->selectedItems().at(0);
  OntologyGraphElement *element = NULL;
  if (selectedItem->data(kIDTType) == kITNode) {
    element = static_cast<OntologyGraphElement *>(static_cast<NodeItem *>(selectedItem));
  }
  else if (selectedItem->data(kIDTType) == kITRelation) {
    element = static_cast<OntologyGraphElement *>(static_cast<RelationItem *>(selectedItem));
  }

  Ui::AttributeEditor editor = Ui::AttributeEditor();

  QDialog *dialog = new QDialog(this);
  editor.setupUi(dialog);
  editor.textEdit->appendPlainText(element->attributesAsText());

  if (dialog->exec() == QDialog::Accepted) {
    element->setAttributesFromData(editor.textEdit->toPlainText().toLocal8Bit());
  }
}

void OntologyWidget::setSolidDashPatternSlot() {

  QGraphicsItem *selectedItem = m_ontologyView->scene()->selectedItems().at(0);
  OntologyGraphElement *element = static_cast<OntologyGraphElement *>(static_cast<RelationItem *>(selectedItem));
  QVariantMap attributesMap = element->attributes();
  QVariantMap dashMap;
  dashMap["dash"] = "solid";
  attributesMap["gui-attributes"] = dashMap;
  element->setAttributes(attributesMap);
}

void OntologyWidget::setDashDashPatternSlot() {

  QGraphicsItem *selectedItem = m_ontologyView->scene()->selectedItems().at(0);
  OntologyGraphElement *element = static_cast<OntologyGraphElement *>(static_cast<RelationItem *>(selectedItem));
  QVariantMap attributesMap = element->attributes();
  QVariantMap dashMap;
  dashMap["dash"] = "dash";
  attributesMap["gui-attributes"] = dashMap;
  element->setAttributes(attributesMap);
}

void OntologyWidget::setDotDashPatternSlot() {

  QGraphicsItem *selectedItem = m_ontologyView->scene()->selectedItems().at(0);
  OntologyGraphElement *element = static_cast<OntologyGraphElement *>(static_cast<RelationItem *>(selectedItem));
  QVariantMap attributesMap = element->attributes();
  QVariantMap dashMap;
  dashMap["dash"] = "dot";
  attributesMap["gui-attributes"] = dashMap;
  element->setAttributes(attributesMap);
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

void OntologyWidget::dataChangedSlot() {

  updateData();
}

void OntologyWidget::itemSelectedSlot(long id) {

  m_ontologyView->scene()->clearSelection();

  foreach (QGraphicsItem *item, m_ontologyView->scene()->items()) {
    if (item->data(kIDTType) == kITNode) {
      NodeItem *nodeItem = static_cast<NodeItem *>(item);
      if (nodeItem->id() == id) {
        item->setSelected(true);
        m_ontologyView->centerOn(item);
      }
    }
    else if (item->data(kIDTType) == kITRelation) {
      RelationItem *relationItem = static_cast<RelationItem *>(item);
      if (relationItem->id() == id) {
        item->setSelected(true);
        m_ontologyView->centerOn(item);
      }
    }
  }
}

void OntologyWidget::itemsSelectedSlot(const QSet<long> ids) {

  m_ontologyView->scene()->clearSelection();

  bool alreadyCentered = false;
  foreach (QGraphicsItem *item, m_ontologyView->scene()->items()) {
    if (item->data(kIDTType) == kITNode) {
      NodeItem *nodeItem = static_cast<NodeItem *>(item);
      if (ids.contains(nodeItem->id())) {
        item->setSelected(true);
        if (!alreadyCentered) {
          m_ontologyView->centerOn(item);
          alreadyCentered = true;
        }
      }
    }
//    else if (item->data(kIDTType) == kITRelation) {
//      RelationItem *relationItem = static_cast<RelationItem *>(item);
//      if (relationItem->id() == id) {
//        item->setSelected(true);
//        m_ontologyView->centerOn(item);
//      }
//    }
  }
}

void OntologyWidget::zoomInSlot() {

  m_ontologyView->scale(1.2, 1.2);
}

void OntologyWidget::zoomOutSlot() {

  m_ontologyView->scale(0.8, 0.8);
}

void OntologyWidget::nodeItemPositionChangedSlot(long id, const QPointF &newPosition) {

  m_dataController->setNodePosition(id, newPosition);
}

QImage OntologyWidget::makeScreenshot() const {

  QImage image(m_ontologyView->scene()->width(), m_ontologyView->scene()->height(), QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&image);
  m_ontologyView->scene()->render(&painter);
  return image;
}

void OntologyWidget::showOntologySlot() {

  if (m_sourceCodeShown) {
    m_sourceCodeShown = false;
    m_sourceCodeViewer->setVisible(false);
    ui->verticalLayout->removeWidget(m_sourceCodeViewer);
    ui->verticalLayout->addWidget(m_ontologyView);
    m_ontologyView->setVisible(true);
  }
}

void OntologyWidget::showSourceCodeSlot() {

  if (!m_sourceCodeShown) {
    m_sourceCodeShown = true;
    if (m_dataController != NULL) {
      qDebug() << m_dataController->sourceCode();
      m_sourceCodeViewer->setText(m_dataController->sourceCode());
    }

    m_ontologyView->setVisible(false);
    ui->verticalLayout->removeWidget(m_ontologyView);
    ui->verticalLayout->addWidget(m_sourceCodeViewer);
    m_sourceCodeViewer->setVisible(true);
  }
}

void OntologyWidget::dropEvent(QDropEvent *event) {

  QByteArray jsonData = event->mimeData()->data("application/node-attrs");
  QPointF globalPos = m_ontologyView->mapToScene(event->pos());
  NodeItem *newNode = addNode(globalPos);
  newNode->setAttributesFromData(jsonData);
}

void OntologyWidget::dragEnterEvent(QDragEnterEvent *event) {

  if (event->mimeData()->hasFormat("application/node-attrs")) {
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}
