#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QObject>
#include "qcustomplot.h"
#include "plot.hpp"
#include <QToolBar>
#include <vector>

#include <libps3000a-1.1/ps3000aApi.h>
#ifndef PICO_STATUS
#include <libps3000a-1.1/PicoStatus.h>
#include <string>
#endif


inline bool    g_videoIsRunning;
inline int32_t g_videoCounter;
inline int32_t g_frameCounter;


struct Buttons : public QRadioButton
{
  Buttons();
  std::vector<QRadioButton*>      Volt_Buttons;
  std::vector<std::string>        Volt_Buttons_Names = {"10mV", "20mV", "50mV", "0.1V", "0.2V", "0.5V", "1V", "2V", "5V", "10V", "20V"};

  std::vector<QRadioButton*>      Mode_Buttons;
  std::vector<std::string>        Mode_Buttons_Names = {"OFF", "X", "Y", "Z"};
};




struct VoltageButtonGroupbox : public QGroupBox
{
                        VoltageButtonGroupbox();
  Buttons *             Buttons_Obj;
  QGridLayout *         Group_Layout;
};


struct ModeButtonGroupbox : public QGroupBox
{
                        ModeButtonGroupbox();
  Buttons *             Buttons_Obj;
  QGridLayout *         Group_Layout;
};




class XYZ_PicoChannelWindow : public QWidget
{
  Q_OBJECT

public:
  XYZ_PicoChannelWindow(Plot *);
  void                        Update_Window();
  void                        Get_Offset_Range();

private:
  UNIT *                      unit;
  Plot *                      plot;
  QGridLayout *               layout;
  QCheckBox *                 Enabled_CheckBox_Obj[PS3000A_MAX_CHANNELS];
  VoltageButtonGroupbox *     VoltageButtonGroupbox_Obj[PS3000A_MAX_CHANNELS];
  QDoubleSpinBox *            Offset_SpinBox_Obj[PS3000A_MAX_CHANNELS];
  ModeButtonGroupbox *        ModeButtonGroupbox_Obj[PS3000A_MAX_CHANNELS];
  QPushButton *               Update_Button;

signals:
  void update_XY_Axis();

public slots:
  void                        Update_Button_Slot();

};




class Window : public QMainWindow
{
  Q_OBJECT
public:
  Window(Plot *);

private:
  int                     counter;
  int                     period;

  QCustomPlot             customPlot;
  QCPColorMap *           colorMap;

  Plot *                  plot;
  UNIT *                  unit;

  QToolBar *              toolBar;
  QPushButton *           streamButton;
  QPushButton *           saveButton;
  QPushButton *           videoButton;
  QSpinBox *              sizeBox;
  QAction *               sizeBoxAction;

  XYZ_PicoChannelWindow *     XYZ_PicoChannelWindow_Obj;

  QAction *               show_XYZ_AxisMenu;

  QAction *               show_XYZ_PicoChannelMenu;



private:
  void setMainWindow();
  void setActions();
  void setConnections();

public slots:
  void update_XY_Axis();

  void setXYZMode();

  void show_XYZ_PicoChannelMenuSlot();

  void data(double, double, double);
  void data(double, double);
  void data(QVector<double>, QVector<double>, int);
  void data(QVector<double>, QVector<double>);

  void changeAxis();
  void resetPlot();

  void streamButton_Slot();
  void saveButton_Slot();
  void videoButton_Slot();
  void setResolution(int);

protected:
#ifndef QT_NO_CONTEXTMENU
  void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

};

#endif //WINDOW_H
