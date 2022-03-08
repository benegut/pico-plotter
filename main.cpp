#include "main.hpp"


int main(int argc, char **argv)
{
  QApplication a(argc, argv);
  Worker worker;
  worker.start();
  Window window(&worker);
  window.show();
  return a.exec();
}
