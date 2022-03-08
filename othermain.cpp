#include <QApplication>
#include "plot.hpp"

int main(int argc, char **argv) {

  QApplication app(argc, argv);
  Plot plot;
  return app.exec();
}
