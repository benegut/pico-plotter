#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QObject>
#include "qcustomplot.h"
#include "plot.hpp"
#include <QToolBar>

#include <libps3000a-1.1/ps3000aApi.h>
#ifndef PICO_STATUS
#include <libps3000a-1.1/PicoStatus.h>
#endif

class Window : public QMainWindow
{
  Q_OBJECT
public:
  Window(Plot *);

private:
  int                     counter;
  int                     period;
  QCustomPlot             customPlot;
  Plot *                  plot;
  QCPColorMap *           colorMap;
  QToolBar *              toolBar;

  QSpinBox *              sizeBox;
  QSpinBox *              periodBox;

  QAction *               resetCanvasAction;
  QAction *               setCyclePeriodUpAction;
  QAction *               setCyclePeriodDownAction;
  QAction *               setResetCycleStartAction;
  QAction *               setResolutionAction;

public slots:
  void setXYMode(UNIT *);
  void setXYZMode(UNIT *);
  void setNormalMode(UNIT *);
  void data(double, double, double);
  void data(double, double);
  void data(QVector<double>, QVector<double>, int);
  void data(QVector<double>, QVector<double>);
  void changeAxis(UNIT *);
  void resetPlot(UNIT *);

  void resetCanvas();
  void setPeriod(int);
  void setResetCycleStart();
  void setResolution(int);

private:
  void setMainWindow();
  void setActions();
  void setConnections();

};

#endif //WINDOW_H
