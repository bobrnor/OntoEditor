#include "PalleteWidget.h"

#include "qjson/include/qjson/serializer.h"

#include <QStringList>
#include <QMimeData>

PalleteWidget::PalleteWidget(QWidget *parent) : QListWidget(parent) {

  setIconSize(QSize(110, 110));
  setSelectionMode(QAbstractItemView::SingleSelection);
  setViewMode(QListView::IconMode);

  QStringList names;
  QStringList shapeNames;
  QStringList picNames;
  names << "Rect" << "Rounded rect";
  shapeNames << "rect" << "rounded_rect";
  picNames << "rect.png" << "rounded-rect.png";

  for (int i = 0; i < names.size(); ++i) {
    QString name = names[i];
    QString shapeName = shapeNames[i];
    QString picName = picNames[i];

    QVariantMap guiAttrs;
    QVariantMap attrs;
    attrs["shape_name"] = shapeName;
    guiAttrs["gui-attributes"] = attrs;

    QListWidgetItem *item = new QListWidgetItem(name, this);
    item->setIcon(QPixmap("/Users/bobrnor/Dropbox/PSU/Projects/OntoEditor/pics/" + picName));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    item->setData(Qt::UserRole, guiAttrs);
  }
}

QStringList PalleteWidget::mimeTypes() {

  return QStringList("application/node-attrs");
}

QMimeData *PalleteWidget::mimeData(const QList<QListWidgetItem *> items) const {

  QMimeData *data = QListWidget::mimeData(items);
  QVariant attrs = items[0]->data(Qt::UserRole);
  QJson::Serializer serializer;
  data->setData("application/node-attrs", serializer.serialize(attrs));
  return data;
}
