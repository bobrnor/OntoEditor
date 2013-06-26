#ifndef PALLETEWIDGET_H
#define PALLETEWIDGET_H

#include <QListWidget>

class PalleteWidget : public QListWidget {

    Q_OBJECT
  public:
    explicit PalleteWidget(QWidget *parent = 0);
    
  protected:
    QStringList mimeTypes();
    QMimeData *mimeData(const QList<QListWidgetItem *> items) const;

  signals:
    
  public slots:
    
};

#endif // PALLETEWIDGET_H
