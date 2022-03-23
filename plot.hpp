#ifndef PLOT_HPP
#define PLOT_HPP

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <libps3000a-1.1/ps3000aApi.h>
#ifndef PICO_STATUS
#include <libps3000a-1.1/PicoStatus.h>
#endif

#include <QThread>
#include <QApplication>
#include <QObject>
#include <vector>
#include <string>





typedef enum
  {
    OFF, X, Y, Z
  }XYMODE;

inline const char* xymode_txt[] = {"OFF", "X", "Y", "Z"};





typedef struct
{
  int16_t range;
  bool    enabled;
  bool    bufferEnabled;
  XYMODE  xymode;
  float   offset;
  float   maxOffset;
  float   minOffset;
}CHANNEL_SETTINGS;





typedef struct t_unit
{
  int16_t                   handle;
  PS3000A_RANGE             firstRange;
  PS3000A_RANGE             lastRange;
  int16_t                   channelCount;
  int16_t                   maxValue;
  CHANNEL_SETTINGS          channelSettings [PS3000A_MAX_CHANNELS];
}UNIT;

Q_DECLARE_METATYPE(UNIT)



inline uint16_t inputRanges [PS3000A_MAX_RANGES] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000};

inline std::vector<double> minOffsetArray;
inline std::vector<double> maxOffsetArray;



inline bool            g_ready;
inline bool            g_streamIsRunning;
inline uint32_t        g_startIndex;
inline int32_t         g_sampleCount;




typedef struct
{
  UNIT *          unit;
  int16_t **      driverBuffers;
  int16_t **      appBuffers;
}BUFFER_INFO;





class Worker : public QObject
{
  Q_OBJECT

public:
  UNIT *          unit;


public:
                  Worker();
  void            start();


private:
  void            get_info();
  void            setChannels();
  void            setDefaults();
  void            getOffsetArrays();
  PICO_STATUS     changePowerSource(int16_t, PICO_STATUS);
  static void     callBackStreaming(int16_t, int32_t,
                                    uint32_t,int16_t,
                                    uint32_t,int16_t,
                                    int16_t, void *);


public slots:
  void            get_Unit_Data_Slot(UNIT);
  void            streamDataHandler();


signals:
  void            sendData(double, double, double);
  void            send_Unit_Data_Signal(UNIT);

};



#endif //PLOT_HPP
