#include <QProcess>
#include <QApplication>
#include <QStringList>
#include "plot.hpp"
#include <QThread>
#include "window.hpp"


int main(int argc, char **argv) {

  QApplication app(argc, argv);
  Plot plot;
  plot.start();
  Window window(&plot);
  window.show();
  return app.exec();
}
