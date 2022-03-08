#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QObject>
#include "qcustomplot.h"
#include "plot.hpp"

class Window : public QMainWindow
{
  Q_OBJECT
public:
  Window(Plot *);

private:
  QCustomPlot             customPlot;
  Plot *                  plot;
  QVector<double>         x = {0, 1};
  QVector<double>         y = {0, 1};


public slots:
  void data(int, int);
};

#endif //WINDOW_H
