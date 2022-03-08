#include <QThread>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <QMainWindow>
#include <QApplication>
#include "qcustomplot.h"


class Worker : public QThread
{
public:
  Worker()
  {
  };

  void run(){
    for(int i = 0; i< 1000000; i++)
      {
        std::cout << i << std::endl;
        emit(sendData(i, 5));
        usleep(1000000);
      };
  };

signals:
    void sendData(int, int){};
};


class Window : public QMainWindow
{
public:
  Window(Worker *p) : worker(p)
  {
    connect(worker, SIGNAL(Worker::sendData(int, int)), this, SLOT(Window::data(int, int)));
    setCentralWidget(&customPlot);
    customPlot.addGraph();
    //customPlot.graph(0)->setData(x, y);
    customPlot.replot();
  };

private:
  QVector<int> x;
  QVector<int> y;
  QCustomPlot customPlot;
  Worker *worker;


public slots:
  void data(int key, int val){
    customPlot.graph(0)->addData(key, val);
    customPlot.replot();
    customPlot.show();
    std::cout << "check\n";
  };
};



int main(int argc, char **argv)
{
  QApplication a(argc, argv);
  Worker worker;
  worker.start();
  Window window(&worker);
  window.show();
  return a.exec();
}
