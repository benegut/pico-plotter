#include <QMainWindow>
#include <QObject>
#include "qcustomplot.h"


class Window : public QMainWindow
{
public:
  Window();

private:
  QCustomPlot customPlot;
  QVector<double> x = {0, 1};
  QVector<double> y = {0, 1};


public slots:
  void data(int, int);
};
