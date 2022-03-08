#include <QThread>
#include <iostream>
#include <chrono>
#include <QThread>
#include <unistd.h>
#include <QMainWindow>
#include <QApplication>
#include "qcustomplot.h"
#include <QObject>


class Worker : public QThread
{
  Q_OBJECT

public:
  void run(){
    for(int i = 0; i< 1000000; i++)
      {
        std::cout << i << std::endl;
        emit(sendData(i, 5));
        usleep(10);
      };
  };

signals:
    void sendData(int, int);
};




class Window : public QMainWindow
{
  Q_OBJECT

public:
  Window(Worker *p) : worker(p)
  {
    connect(worker, SIGNAL(sendData(int, int)), this, SLOT(data(int, int)));
    setCentralWidget(&customPlot);
    customPlot.addGraph();
    customPlot.graph(0)->addData(0,0);
    customPlot.xAxis->setRange(0, 1000000);
    customPlot.yAxis->setRange(0, 1000000);
    customPlot.replot();
  };

private:
  QCustomPlot customPlot;
  Worker *worker;


public slots:
  void data(int key, int val){
    customPlot.graph(0)->addData(key, key);
    if(key%100 == 0){customPlot.replot();}
    
  };
};

