#include "window.hpp"
#include "qcustomplot.h"
#include <cstdio>
#include <QDateTime>
#include <QString>


Window::Window(Plot * p)
  : plot(p)
  , unit(p->unit)
  , colorMap(new QCPColorMap(customPlot.xAxis, customPlot.yAxis))
  , toolBar(new QToolBar())
  , counter(0)
  , period(100)
{
  XYZ_PicoChannelWindow_Obj = new XYZ_PicoChannelWindow(plot);

  customPlot.setInteraction(QCP::iRangeDrag, true);
  customPlot.setInteraction(QCP::iRangeZoom, true);

  setMainWindow();
  setActions();
  setConnections();

  g_videoIsRunning = false;
  g_videoCounter = 0;
  g_frameCounter = 0;
}


void Window::setMainWindow()
{
  setCentralWidget(&customPlot);
  resize(600, 600);
  addToolBar(toolBar);
}


void Window::setActions()
{
  streamButton = new QPushButton();
  streamButton->setText(g_autoStart ? "&Stop" : "&Start");
  toolBar->addWidget(streamButton);

  saveButton = new QPushButton(tr("&Save"));
  toolBar->addWidget(saveButton);

  videoButton = new QPushButton(tr("&Video"));
  toolBar->addWidget(videoButton);

  sizeBox = new QSpinBox();
  sizeBox->setMaximum(300);
  sizeBox->setMinimum(50);
  sizeBox->setSingleStep(10);
  sizeBox->setValue(200);
  sizeBoxAction = toolBar->addWidget(sizeBox);
  sizeBoxAction->setVisible(false);

  show_XYZ_AxisMenu = new QAction(tr("&Axis"));
  show_XYZ_AxisMenu->setVisible(false);
  menuBar()->addAction(show_XYZ_AxisMenu);

  show_XYZ_PicoChannelMenu = new QAction(tr("&Channel"));
  show_XYZ_PicoChannelMenu->setVisible(false);
  menuBar()->addAction(show_XYZ_PicoChannelMenu);
}


void Window::setConnections()
{
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>, int)), this, SLOT(data(QVector<double>, QVector<double>, int)));
  connect(plot, SIGNAL(sendData(double, double, double)), this, SLOT(data(double, double, double)));
  connect(plot, SIGNAL(sendData(QVector<double>, QVector<double>)), this, SLOT(data(QVector<double>, QVector<double>)));
  connect(plot, SIGNAL(sendData(double, double)), this, SLOT(data(double, double)));

  connect(plot, SIGNAL(setXYZMode()), this, SLOT(setXYZMode()));

  connect(plot, SIGNAL(changeAxis()), this, SLOT(changeAxis()));
  connect(plot, SIGNAL(resetPlot()), this, SLOT(resetPlot()));

  connect(streamButton, SIGNAL(clicked()), this, SLOT(streamButton_Slot()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButton_Slot()));
  connect(videoButton, SIGNAL(clicked()), this, SLOT(videoButton_Slot()));
  connect(sizeBox, SIGNAL(valueChanged(int)), this, SLOT(setResolution(int)));

  connect(show_XYZ_PicoChannelMenu, SIGNAL(triggered()), this, SLOT(show_XYZ_PicoChannelMenuSlot()));

  connect(XYZ_PicoChannelWindow_Obj, SIGNAL(update_XY_Axis()), this, SLOT(update_XY_Axis()));
}



#ifndef QT_NO_CONTEXTMENU
void Window::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);
  menu.exec(event->globalPos());
}
#endif //QT_NO_CONTEXTMENU



void Window::update_XY_Axis()
{
  int X_Range = 0;
  int Y_Range = 0;

  for (int ch=0; ch < unit->channelCount; ch++)
    {
      if(unit->channelSettings[ch].enabled &&
         unit->channelSettings[ch].xymode == X)
        {
          int dum = unit->firstRange + unit->channelSettings[ch].range;
          X_Range = inputRanges[dum] > X_Range ? inputRanges[dum] : X_Range;
        }
      else if(unit->channelSettings[ch].enabled &&
              unit->channelSettings[ch].xymode == Y)
        {
          int dum = unit->firstRange + unit->channelSettings[ch].range;
          Y_Range = inputRanges[dum] > Y_Range ? inputRanges[dum] : Y_Range;
        }
    }

  if(!X_Range || !Y_Range)
    printf("\n*****X and Y mode must be selected*****\n");

  colorMap->data()->setRange(QCPRange(-X_Range,X_Range), QCPRange(-Y_Range, Y_Range));
  customPlot.rescaleAxes();
  customPlot.replot();
}



void Window::setXYZMode()
{
  customPlot.axisRect()->setupFullAxesBox(true);
  colorMap->data()->setSize(200, 200);

  update_XY_Axis();

  colorMap->setDataRange(QCPRange(-1,1));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  colorMap->rescaleDataRange();
  colorMap->data()->fill(0.0);
  customPlot.rescaleAxes();
  customPlot.replot();

  sizeBoxAction->setVisible(true);

  show_XYZ_AxisMenu->setVisible(true);
  show_XYZ_PicoChannelMenu->setVisible(true);

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
      if(g_videoIsRunning)
        {
          customPlot.savePng("videos/" + QString::number(g_videoCounter) + "/" + QString::number(g_frameCounter) + ".png");
          g_frameCounter++;
        }
    }
  if(counter%period == 0)
    colorMap->data()->fill(1.0);
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


void Window::changeAxis()
{
  unit->timeBaseStart = unit->timeBaseEnd;
  unit->timeBaseEnd += unit->timeBase;
  customPlot.xAxis->setRange(unit->timeBaseStart, unit->timeBaseEnd);
  customPlot.replot();
}


void Window::resetPlot()
{
  unit->timeBaseStart = 0;
  unit->timeBaseEnd = unit->timeBase;
  customPlot.xAxis->setRange(unit->timeBaseStart, unit->timeBaseEnd);
  customPlot.clearGraphs();
}


void Window::streamButton_Slot()
{
  g_autoStart = !g_autoStart;
  streamButton->setText(g_autoStart ? "&Stop" : "&Start");
}


void Window::setResolution(int size)
{
  colorMap->data()->setSize(size, size);
  customPlot.replot();
}


void Window::show_XYZ_PicoChannelMenuSlot()
{
  XYZ_PicoChannelWindow_Obj->Update_Window();
  XYZ_PicoChannelWindow_Obj->show();
}


void Window::saveButton_Slot()
{
  customPlot.savePng("images/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png");
}


void Window::videoButton_Slot()
{
  if(!g_videoIsRunning)
    g_frameCounter = 0;

  while(!g_videoIsRunning &&
        QDir("videos/" + QString::number(g_videoCounter)).exists())
    g_videoCounter++;
  QDir().mkdir("videos/" + QString::number(g_videoCounter));

  g_videoIsRunning = !g_videoIsRunning;

  videoButton->setText(g_videoIsRunning ? "&Running..." : "&Video");
}
