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
#include <cstring>
#include <vector>
#include <queue>
#include <string>

#define memcpy_s(a, b, c, d) memcpy(a, c, d)

typedef enum
  {
    OFF, X, Y, Z
  }XYMODE;


inline const char* xymode_txt[] = {"OFF", "X", "Y", "Z"};


typedef struct
{
  int16_t range;
  int16_t enabled;
  int16_t bufferEnabled;
  XYMODE  xymode;
  int16_t graph;
  float   offset;
  float   maxOffset;
  float   minOffset;
}CHANNEL_SETTINGS;


typedef struct
{
  int16_t                   handle;
  int8_t                    model[8];
  PS3000A_RANGE             firstRange ;
  PS3000A_RANGE             lastRange;
  int16_t                   channelCount;
  int16_t                   maxValue;
  int32_t                   timeBase;
  int32_t                   timeBaseStart;
  int32_t                   timeBaseEnd;
  CHANNEL_SETTINGS          channelSettings [PS3000A_MAX_CHANNELS];
}UNIT;



inline uint16_t inputRanges [PS3000A_MAX_RANGES] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000};

inline std::vector<double> minOffsetArray;
inline std::vector<double> maxOffsetArray;


typedef struct tBufferInfo
{
  UNIT *          unit;
  int16_t **      driverBuffers;
  int16_t **      appBuffers;
} BUFFER_INFO;



inline bool            g_ready;
inline bool            g_autoStart;
inline bool            g_autoStopped;
inline uint32_t        g_startIndex;
inline int32_t         g_sampleCount;



class Plot : public QThread
{
  Q_OBJECT

signals:
  void setXYZMode();

  void sendData(double, double, double);
  void sendData(double, double);
  void sendData(QVector<double>, QVector<double>, int);
  void sendData(QVector<double>, QVector<double>);

  void changeAxis();
  void resetPlot();

private:
  int32_t         _getch();
  int32_t         _kbhit();

  PICO_STATUS     changePowerSource(int16_t, PICO_STATUS);
  static void     callBackStreaming(int16_t, int32_t,
                                    uint32_t,int16_t,
                                    uint32_t,int16_t,
                                    int16_t, void *);
  void            streamDataHandler(int);
  void            get_info();
  void            setChannels();
  PICO_STATUS     openDevice();
  void            closeDevice();
  void            getOffsetArrays();
  void            run();

public:
  void            setDefaults();

public:
  std::queue<std::vector<int>>          xyLine;
  int16_t                               xyLineSize;

  UNIT *                                unit;
};


#endif //PLOT_HPP
