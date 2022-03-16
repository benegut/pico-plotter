#include "window.hpp"
#include "qcustomplot.h"

Window::Window(Plot * p)
  : plot(p)
{
  setCentralWidget(&customPlot);
  resize(600, 600);
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>, int)), this, SLOT(data(QVector<double>, QVector<double>, int)));
  connect(plot, SIGNAL(sendData(double, double, double)), this, SLOT(data(double, double, double)));
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>)), this, SLOT(data(QVector<double>, QVector<double>)));
  connect(plot, SIGNAL(sendData(double, double)), this, SLOT(data(double, double)));
  connect(plot, SIGNAL(setXYMode(UNIT *)), this, SLOT(setXYMode(UNIT *)));
  connect(plot, SIGNAL(setXYZMode(UNIT *)), this, SLOT(setXYZMode(UNIT *)));
  connect(plot, SIGNAL(setNormalMode(UNIT *)), this, SLOT(setNormalMode(UNIT *)));
  connect(plot, SIGNAL(changeAxis(UNIT *)), this, SLOT(changeAxis(UNIT *)));
  connect(plot, SIGNAL(resetPlot(UNIT *)), this, SLOT(resetPlot(UNIT *)));
}


void Window::setXYMode(UNIT * unit)
{
  customPlot.axisRect()->setupFullAxesBox(true);
  int range = 0;
  for (int i=0; i < unit->channelCount; i++)
    {
      if(unit->channelSettings[i].enabled)
        {
          int dum = unit->firstRange + unit->channelSettings[i].range;
          range = inputRanges[dum] > range ? inputRanges[dum] : range;
        }
    }
  colorMap->data()->setSize(210, 210);
  colorMap->data()->setRange(QCPRange(-range,range), QCPRange(-range, range));
  colorMap->setDataRange(QCPRange(-range,range));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  customPlot.rescaleAxes();
  customPlot.replot();
  this->show();

  counter = 0;
}


void Window::setXYZMode(UNIT * unit)
{
  customPlot.axisRect()->setupFullAxesBox(true);
  colorMap->data()->setSize(200, 200);
  int range = 0;
  for (int i=0; i < unit->channelCount; i++)
    {
      if(unit->channelSettings[i].enabled)
        {
          int dum = unit->firstRange + unit->channelSettings[i].range;
          range = inputRanges[dum] > range ? inputRanges[dum] : range;
        }
    }
  colorMap->data()->setRange(QCPRange(-range,range), QCPRange(-range, range));
  colorMap->setDataRange(QCPRange(-1,1));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  colorMap->rescaleDataRange();
  customPlot.rescaleAxes();
  customPlot.replot();
  this->show();

  counter = 0;
}


void Window::setNormalMode(UNIT * unit)
{
  customPlot.xAxis->setRange(unit->timeBaseStart, unit->timeBaseEnd);
  int range = 0;
  for (int i=0; i < unit->channelCount; i++)
    {
      if(unit->channelSettings[i].enabled)
        {
          int graph = unit->channelSettings[i].graph;
          customPlot.addGraph();
          Qt::GlobalColor color = (Qt::GlobalColor)(graph+8);
          QColor penColor(color);
          penColor.setAlpha(100);
          customPlot.graph(graph)->setPen(QPen(penColor));
          int dum = unit->firstRange + unit->channelSettings[i].range;
          range = inputRanges[dum] > range ? inputRanges[dum] : range;
        }
    }
  customPlot.yAxis->setRange(-range, range);
  customPlot.replot();
  this->show();
}


void Window::data(double x, double y, double z)                   //Maybe merge those to with default z = 10000.0 or smth.
{
  int xInd, yInd;
  colorMap->data()->coordToCell(x, y, &xInd, &yInd);
  colorMap->data()->setCell(xInd, yInd, z);
  counter++;
  if(counter%1000 == 0)
    {
      customPlot.replot();
      customPlot.show();
    }
  if(counter%50000 == 0)
    colorMap->data()->fill(0.0);
}


void Window::data(double x, double y)
{
  int xInd, yInd;
  colorMap->data()->coordToCell(x, y, &xInd, &yInd);
  colorMap->data()->setCell(xInd, yInd, 100000.0);
  if(counter < plot->xyLineSize)
    plot->xyLine.push({xInd, yInd});
  else
    {
      colorMap->data()->setCell(plot->xyLine.front()[0], plot->xyLine.front()[1], 0);
      plot->xyLine.pop();
      plot->xyLine.push({xInd, yInd});
    }
  counter++;
  if(counter%100 == 0)
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


void Window::changeAxis(UNIT * unit)
{
  unit->timeBaseStart = unit->timeBaseEnd;
  unit->timeBaseEnd += unit->timeBase;
  customPlot.xAxis->setRange(unit->timeBaseStart, unit->timeBaseEnd);
  customPlot.replot();
}


void Window::resetPlot(UNIT * unit)
{
  unit->timeBaseStart = 0;
  unit->timeBaseEnd = unit->timeBase;
  customPlot.xAxis->setRange(unit->timeBaseStart, unit->timeBaseEnd);
  customPlot.clearGraphs();
  setNormalMode(unit);
}

