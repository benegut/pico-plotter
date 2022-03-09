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

public slots:
  void data(int, int);
  void data(QVector<double>, QVector<double>, int);
};

#endif //WINDOW_H
