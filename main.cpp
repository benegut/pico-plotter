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
  Window window;
  QThread::connect(&plot, SIGNAL(sendData(int)), &window, SLOT(data(int)));
  window.show();
  return app.exec();
}
