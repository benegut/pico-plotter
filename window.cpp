#include "window.hpp"
#include "qcustomplot.h"
#include <cstdio>
#include <QDateTime>
#include <QString>
#include <QCloseEvent>


Window::Window()
  : customPlot(new QCustomPlot())
  , colorMap(new QCPColorMap(customPlot->xAxis, customPlot->yAxis))
  , toolBar(new QToolBar())
{
  unit = new UNIT();
  Worker_Obj = new Worker();
  Worker_Obj->moveToThread(&Thread_Obj);
  Thread_Obj.start();
  connect(Worker_Obj, SIGNAL(send_Unit_Data_Signal(UNIT)), this, SLOT(get_Unit_Data_Slot(UNIT)));
  Worker_Obj->start();

  XYZ_PicoChannelWindow_Obj = new XYZ_PicoChannelWindow(unit);

  set_MainWindow();
  set_Actions();
  set_Connections();

  g_videoIsRunning  = false;
  g_videoCounter    = 0;
  g_frameCounter    = 0;
  g_streamIsRunning = false;

  counter           = 0;
  period            = 500;

}





void Window::start()
{
  setXYZMode();
}





void Window::set_MainWindow()
{
  setCentralWidget(customPlot);
  resize(600, 600);
  addToolBar(toolBar);
}





void Window::set_Actions()
{
  streamButton = new QPushButton();
  streamButton->setText(g_streamIsRunning ? "&Stop" : "&Start");
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

  show_XYZ_PicoChannelMenu = new QAction(tr("&Channel"));
  menuBar()->addAction(show_XYZ_PicoChannelMenu);

  qRegisterMetaType<UNIT>();
}





void Window::set_Connections()
{
  connect(this, SIGNAL(send_Unit_Data_Signal(UNIT)), Worker_Obj, SLOT(get_Unit_Data_Slot(UNIT)));
  connect(Worker_Obj, SIGNAL(send_Unit_Data_Signal(UNIT)), this, SLOT(get_Unit_Data_Slot(UNIT)));
  connect(Worker_Obj, SIGNAL(sendData(double, double, double)), this, SLOT(data(double, double, double)));
  connect(this, SIGNAL(start_Stream_Signal()), Worker_Obj, SLOT(streamDataHandler()));
  connect(this, SIGNAL(stop_Stream_Signal()), this, SLOT(stop_Stream_Slot()));

  connect(streamButton, SIGNAL(clicked()), this, SLOT(streamButton_Slot()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButton_Slot()));
  connect(videoButton, SIGNAL(clicked()), this, SLOT(videoButton_Slot()));
  connect(sizeBox, SIGNAL(valueChanged(int)), this, SLOT(setResolution(int)));

  connect(show_XYZ_PicoChannelMenu, SIGNAL(triggered()), this, SLOT(show_XYZ_PicoChannelMenuSlot()));

  connect(XYZ_PicoChannelWindow_Obj, SIGNAL(update_XY_Axis()), this, SLOT(update_XY_Axis()));
  connect(XYZ_PicoChannelWindow_Obj, SIGNAL(start_Stream_Signal()), Worker_Obj, SLOT(streamDataHandler()));
  connect(XYZ_PicoChannelWindow_Obj, SIGNAL(stop_Stream_Signal()), this, SLOT(stop_Stream_Slot()));
  connect(XYZ_PicoChannelWindow_Obj, SIGNAL(send_Unit_Data_Signal(UNIT)), Worker_Obj, SLOT(get_Unit_Data_Slot(UNIT)));
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
  customPlot->rescaleAxes();
  customPlot->replot();
}





void Window::setXYZMode()
{
  unit->channelSettings[0].xymode        = X;
  unit->channelSettings[1].xymode        = Y;
  unit->channelSettings[2].xymode        = Z;
  unit->channelSettings[3].xymode        = OFF;

  emit(send_Unit_Data_Signal(*unit));

  customPlot->axisRect()->setupFullAxesBox(true);
  customPlot->setInteraction(QCP::iRangeDrag, true);
  customPlot->setInteraction(QCP::iRangeZoom, true);
  colorMap->data()->setSize(200, 200);

  update_XY_Axis();

  colorMap->setDataRange(QCPRange(-1,1));
  colorMap->setGradient(QCPColorGradient::gpGrayscale);
  colorMap->data()->fill(0.0);
  customPlot->replot();

  this->show();
}





void Window::data(double x, double y, double z)
{
  int xInd, yInd;
  colorMap->data()->coordToCell(x, y, &xInd, &yInd);
  colorMap->data()->setCell(xInd, yInd, z);
  counter++;
  if(counter%1000 == 0)
    {
      customPlot->replot();
      customPlot->show();
      if(g_videoIsRunning)
        {
          customPlot->savePng("videos/" + QString::number(g_videoCounter) + "/" + QString::number(g_frameCounter) + ".png");
          g_frameCounter++;
        }
    }
  if(counter%period == 0)
    colorMap->data()->fill(1.0);
}





void Window::streamButton_Slot()
{
  if(!g_streamIsRunning)
    {
      emit(start_Stream_Signal());
      g_streamIsRunning = true;
    }
  else if(g_streamIsRunning)
    emit(stop_Stream_Signal());

  streamButton->setText(g_streamIsRunning ? "&Stop" : "&Start");
}





void Window::setResolution(int size)
{
  colorMap->data()->setSize(size, size);
  customPlot->replot();
}





void Window::show_XYZ_PicoChannelMenuSlot()
{
  XYZ_PicoChannelWindow_Obj->Update_Window();
  XYZ_PicoChannelWindow_Obj->show();
}





void Window::saveButton_Slot()
{
  customPlot->savePng("images/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png");
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





void Window::get_Unit_Data_Slot(UNIT u)
{
  *unit = u;
}





void Window::stop_Stream_Slot()
{
  g_streamIsRunning = false;
}





void Window::close_Device()
{
  ps3000aCloseUnit(unit->handle);
}





void Window::closeEvent(QCloseEvent *event)
{
  g_streamIsRunning = false;
  close_Device();
  Thread_Obj.quit();
  Thread_Obj.wait();
  QCoreApplication::quit();
  event->accept();
}
