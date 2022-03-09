#include "window.hpp"
#include "qcustomplot.h"

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  customPlot.addGraph();
  customPlot.addGraph();
  customPlot.addGraph();
  customPlot.addGraph();
  customPlot.graph(0)->addData(0, 0);
  customPlot.graph(1)->addData(0, 0);
  customPlot.graph(2)->addData(0, 0);
  customPlot.graph(3)->addData(0, 0);
  customPlot.graph(0)->setPen(QPen(Qt::blue));
  customPlot.graph(1)->setPen(QPen(Qt::red));
  customPlot.graph(2)->setPen(QPen(Qt::green));
  customPlot.graph(3)->setPen(QPen(Qt::yellow));
  customPlot.graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 1));
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.xAxis->setRange(-50000, 50000);
  customPlot.yAxis->setRange(-50000, 50000);
  customPlot.replot();
  customPlot.show();
  //connect(plot, SIGNAL(sendData(int, int)), this, SLOT(data(int, int)));
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>, int)), this, SLOT(data(QVector<double>, QVector<double>, int)));

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

void Window::data(QVector<double> key, QVector<double> vec, int graph)
{
  customPlot.graph(graph)->addData(key, vec, false);
  customPlot.replot();
}

