#include <QtGui/QApplication>
#include "OntologyWidget.h"

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);
  OntologyWidget w;
  w.show();

  return a.exec();
}
