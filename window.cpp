#include "window.hpp"
#include "qcustomplot.h"
#include <iostream>

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  customPlot.addGraph();
  customPlot.graph(0)->addData(0, 0);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.replot();
  customPlot.show();
  connect(plot, SIGNAL(sendData(int, int)), this, SLOT(data(int, int)));

}

void Window::data(int key, int val)
{
  customPlot.graph(0)->addData(key, val);
  customPlot.replot();
  std::cout << "Check.\n";
}
