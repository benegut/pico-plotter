#include "window.hpp"
#include "qcustomplot.h"

Window::Window()
{
  setCentralWidget(&customPlot);
  customPlot.addGraph();
  customPlot.graph(0)->setData(x, y);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.replot();
}

void Window::data(int key, int val)
{
  customPlot.graph(0)->addData(key, val);
  customPlot.replot();
}
