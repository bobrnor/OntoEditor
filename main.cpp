#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QApplication a(argc, argv);
  MainWindow mw;
  mw.show();

  return a.exec();
}
