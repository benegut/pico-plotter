#include "window.hpp"
#include "qcustomplot.h"

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  customPlot.addGraph();
  customPlot.graph(0)->addData(0, 0);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.xAxis->setRange(0,100000);
  customPlot.yAxis->setRange(-1500, 1500);
  customPlot.replot();
  customPlot.show();
  connect(plot, SIGNAL(sendData(int, int)), this, SLOT(data(int, int)));

}

void Window::data(int key, int val)
{
  customPlot.graph(0)->addData(key, val);
  if(key%100 == 0)
    {
      customPlot.replot();
      customPlot.show();
    }
}
