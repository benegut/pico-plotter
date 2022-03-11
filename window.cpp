#include "window.hpp"
#include "qcustomplot.h"
#include <iostream>

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>, int)), this, SLOT(data(QVector<double>, QVector<double>, int)));
  connect(plot, SIGNAL(sendData(double, double, double)), this, SLOT(data(double, double, double)));
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>)), this, SLOT(data(QVector<double>, QVector<double>)));
  connect(plot, SIGNAL(sendData(int, int)), this, SLOT(data(int, int)));
  connect(plot, SIGNAL(setXYMode(UNIT *)), this, SLOT(setXYMode(UNIT *)));
  connect(plot, SIGNAL(setNormalMode(UNIT *)), this, SLOT(setNormalMode(UNIT *)));
  connect(plot, SIGNAL(setXYLineMode(UNIT *)), this, SLOT(setXYLineMode(UNIT *)));

}


void Window::setXYMode(UNIT * unit)
{
  customPlot.axisRect()->setupFullAxesBox(true);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  colorMap->data()->setSize(200, 200);
  colorMap->data()->setRange(QCPRange(-4,4), QCPRange(-4, 4));

  double x, y, z;
  for (int xIndex=0; xIndex<200; ++xIndex)
    {
      for (int yIndex=0; yIndex<200; ++yIndex)
        {
          colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
          double r = 3*qSqrt(x*x+y*y)+1e-2;
          z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
          colorMap->data()->setCell(xIndex, yIndex, z);
        }
    }

  colorMap->setGradient(QCPColorGradient::gpPolar);
  colorMap->rescaleDataRange();

  QCPMarginGroup *marginGroup = new QCPMarginGroup(&customPlot);
  customPlot.axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  customPlot.rescaleAxes();
  customPlot.replot();
  this->show();
}


void Window::setNormalMode(UNIT * unit)
{
  customPlot.xAxis->setRange(0, 100000);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");

  int graph = -1;
  int range = 0;
  for (int i=0; i < unit->channelCount; i++)
    {
      if(unit->channelSettings[i].enabled)
        {
          graph++;
          customPlot.addGraph();
          customPlot.graph(graph)->setPen(QPen(QColor(((double)graph)*90.0/360.0, 1, 1)));
          int dum = unit->firstRange + unit->channelSettings[i].range;
          range = inputRanges[dum] > range ? inputRanges[dum] : range;
        }
    }
  customPlot.yAxis->setRange(-range, range);
  customPlot.replot();
  this->show();
}


void Window::setXYLineMode(UNIT * unit)
{
  customPlot.axisRect()->setupFullAxesBox(true);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  double range = (double)(unit->maxValue);
  colorMap->data()->setSize(100, 100);
  colorMap->data()->setRange(QCPRange(-range,range), QCPRange(-range, range));
  colorMap->setDataRange(QCPRange(-range,range));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  customPlot.rescaleAxes();
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


void Window::data(int x, int y)
{
  std::cout << x << " " << y << std::endl;
  colorMap->data()->coordToCell(x, y, &x, &y);
  std::cout << x << " " << y << std::endl;
  colorMap->data()->setData(x, y, 100000.0);
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

