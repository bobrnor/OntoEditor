#ifndef ONTOLOGYWIDGET_H
#define ONTOLOGYWIDGET_H

#include <QTextEdit>

#include "lib_json/json/json.h"

#include "OntologyGraphicsView.h"
#include "../../../core/OntologyDataController.h"

class RelationVisualizedLine;
class NodeItem;

namespace Ui {
  class OntologyWidget;
}

class OntologyWidget : public QWidget {
    Q_OBJECT
    
  public:
    explicit OntologyWidget(QWidget *parent = 0);
    ~OntologyWidget();

    void setDataController(OntologyDataController *dataController);
    OntologyDataController *dataController() const;

    QImage makeScreenshot() const;

    void updateData();
    
  private:
    Ui::OntologyWidget *ui;

    QTextEdit *m_sourceCodeViewer;
    OntologyGraphicsView *m_ontologyView;
    QPointF m_lastRightClickScenePosition;
    RelationVisualizedLine *m_relationVisualizedLine;
    OntologyDataController *m_dataController;

    bool m_editRelationMode;
    bool m_sourceCodeShown;

    void setEditRelationMode(bool on);
    void setRelation(NodeItem *sourceNode, NodeItem *destinationNode);

  signals:
    void dataChangedSignal();

  public slots:
    void showContextMenuSlot(const QPoint &pos);

    void addNodeSlot();
    void setRelationSlot();
    void editNodeSlot();
    void editRelationSlot();
    void removeSelectedSlot();
    void editAttrsSlot();

    void sceneSelectionChangedSlot();

    void ontologyViewMousePositionChangedSlot(const QPoint &pos);

    void dataChangedSlot();

    void itemSelectedSlot(long id);
    void itemsSelectedSlot(const QSet<long> ids);

    void zoomInSlot();
    void zoomOutSlot();

    void nodeItemPositionChangedSlot(long id, const QPointF &newPosition);

    void showOntologySlot();
    void showSourceCodeSlot();
};

#endif // ONTOLOGYWIDGET_H
