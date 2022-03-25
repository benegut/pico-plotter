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
                              XYZ_PicoChannelWindow(UNIT *);
  void                        Update_Window();
  void                        Get_Offset_Range();

private:
  UNIT *                      unit;

  QGridLayout *               layout;
  QCheckBox *                 Enabled_CheckBox_Obj[PS3000A_MAX_CHANNELS];
  VoltageButtonGroupbox *     VoltageButtonGroupbox_Obj[PS3000A_MAX_CHANNELS];
  QDoubleSpinBox *            Offset_SpinBox_Obj[PS3000A_MAX_CHANNELS];
  ModeButtonGroupbox *        ModeButtonGroupbox_Obj[PS3000A_MAX_CHANNELS];
  QPushButton *               Update_Button;

signals:
  void                        update_XY_Axis();
  void                        start_Stream_Signal();
  void                        stop_Stream_Signal();
  void                        send_Unit_Data_Signal(UNIT);


public slots:
  void                        Update_Button_Slot();

};





class Time_Plot_Widget : public QWidget
{

};





class Window : public QMainWindow
{
  Q_OBJECT

private:
  Worker *                Worker_Obj;
  QThread                 Thread_Obj;

  UNIT *                  unit;

  QCustomPlot *           customPlot;
  QCPColorMap *           colorMap;

  XYZ_PicoChannelWindow * XYZ_PicoChannelWindow_Obj;
  QAction *               show_XYZ_PicoChannelMenu;

  QToolBar *              toolBar;
  QPushButton *           streamButton;
  QPushButton *           saveButton;
  QPushButton *           videoButton;
  QPushButton *           resetButton;
  QSpinBox *              sizeBox;
  QAction *               sizeBoxAction;
  QDoubleSpinBox *        scaleOffsetBox;
  QAction *               scaleOffsetBoxAction;
  QDoubleSpinBox *        scaleAmplitudeBox;
  QAction *               scaleAmplitudeBoxAction;

  double                  Z_Scale_Offset;
  double                  Z_Scale_Amplitude;

  bool                    g_videoIsRunning;
  int32_t                 g_videoCounter;
  int32_t                 g_frameCounter;

  int                     counter;
  int                     period;


public:
                          Window();
  void                    start();


private:
  void                    set_MainWindow();
  void                    set_Actions();
  void                    set_Connections();
  void                    setXYZMode();

  void                    close_Device();
  void                    closeEvent(QCloseEvent *);


public slots:
  void                    update_XY_Axis();
  void                    show_XYZ_PicoChannelMenuSlot();
  void                    data(double, double, double, double);
  void                    streamButton_Slot();
  void                    saveButton_Slot();
  void                    videoButton_Slot();
  void                    resetButton_Slot();
  void                    setResolution(int);
  void                    set_Z_Scale_Offset(double);
  void                    set_Z_Scale_Amplitude(double);
  void                    get_Unit_Data_Slot(UNIT);

  void                    stop_Stream_Slot();


signals:
  void                    send_Unit_Data_Signal(UNIT);

  void                    start_Stream_Signal();
  void                    stop_Stream_Signal();


protected:
#ifndef QT_NO_CONTEXTMENU
  void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

};



#endif //WINDOW_H
