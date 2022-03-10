#include "window.hpp"
#include "qcustomplot.h"

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>, int)), this, SLOT(data(QVector<double>, QVector<double>, int)));
  connect(plot, SIGNAL(sendData(double, double, double)), this, SLOT(data(double, double, double)));
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>)), this, SLOT(data(QVector<double>, QVector<double>)));
  connect(plot, SIGNAL(setXYMode(UNIT *)), this, SLOT(setXYMode(UNIT *)));
  connect(plot, SIGNAL(setNormalMode(UNIT *)), this, SLOT(setNormalMode(UNIT *)));
  connect(plot, SIGNAL(setXYLineMode(UNIT *)), this, SLOT(setXYLineMode(UNIT *)));
}


void Window::setXYMode(UNIT * unit)
{
  customPlot.axisRect()->setupFullAxesBox(true);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  colorMap->data()->setSize(500, 500);
  colorMap->data()->setRange(QCPRange(-50000,50000), QCPRange(-50000, 50000));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  customPlot.replot();
  this->show();
}


void Window::setNormalMode(UNIT * unit)
{
  customPlot.xAxis->setRange(0, 100000);
  customPlot.yAxis->setRange(-50000, 50000);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");

  int graph = -1;
  for (int i=0; i < unit->channelCount; i++)
    {
      if(unit->channelSettings[i].enabled)
        {
          graph++;
          customPlot.addGraph();
          customPlot.graph(graph)->setPen(QPen(QColor(((double)graph)*90.0/360.0, 1, 1)));
        }
    }
  customPlot.replot();
  this->show();
}


void Window::setXYLineMode(UNIT * unit)
{
  customPlot.xAxis->setRange(-20000, 20000);
  customPlot.yAxis->setRange(-20000, 20000);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.addGraph();
  customPlot.graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 1));
  customPlot.replot();
  this->show();
}


void Window::data(double x, double y, double z)
{
  colorMap->data()->setData((int)x, (int)y, z);
  counter++;

  if(counter%1000 == 0)
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


void Window::data(QVector<double> key, QVector<double> vec)
{
  customPlot.graph(0)->addData(key, vec, false);
  customPlot.replot();
}

