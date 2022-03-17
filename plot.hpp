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
//#include <curses.h>

#include <libps3000a-1.1/ps3000aApi.h>
#ifndef PICO_STATUS
#include <libps3000a-1.1/PicoStatus.h>
#endif

#include <QThread>
#include <QApplication>
#include <QObject>
#include <cstdio>
#include <cstring>
#include <vector>
#include <queue>
#include <string>

#define memcpy_s(a, b, c, d) memcpy(a, c, d)

#define PREF4 __stdcall

typedef enum
  {
    OFF, X, Y, INTENSITY
  }XYMODE;

inline const char* xymode_txt[] = {"OFF", "X", "Y", "INTENSITY"};


typedef struct
{
  int16_t DCcoupled;
  int16_t range;
  int16_t enabled;
  XYMODE  xymode;
  int16_t graph;
  float   offset;
}CHANNEL_SETTINGS;


typedef struct tTriggerDirections
{
  PS3000A_THRESHOLD_DIRECTION channelA;
  PS3000A_THRESHOLD_DIRECTION channelB;
  PS3000A_THRESHOLD_DIRECTION channelC;
  PS3000A_THRESHOLD_DIRECTION channelD;
  PS3000A_THRESHOLD_DIRECTION ext;
  PS3000A_THRESHOLD_DIRECTION aux;
}TRIGGER_DIRECTIONS;


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


inline bool           g_ready = 0;
inline uint32_t       g_startIndex;
inline int16_t        g_autoStopped;
inline int32_t        g_sampleCount;
inline int16_t        g_mode;

inline uint16_t inputRanges [PS3000A_MAX_RANGES] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000};

inline char StreamFile[20]		= "stream.txt";


typedef enum
  {
    ANALOGUE,
    DIGITAL,
    AGGREGATED,
    MIXED
  }MODE;


typedef struct tBufferInfo
{
  UNIT *          unit;
  MODE            mode;
  int16_t **      driverBuffers;
  int16_t **      appBuffers;
} BUFFER_INFO;


class Plot : public QThread
{
  Q_OBJECT

signals:
  void setXYMode(UNIT *);
  void setXYZMode(UNIT *);
  void setNormalMode(UNIT *);
  void sendData(double, double, double);
  void sendData(double, double);
  void sendData(QVector<double>, QVector<double>, int);
  void sendData(QVector<double>, QVector<double>);
  void changeAxis(UNIT *);
  void resetPlot(UNIT *);

private:
  int32_t         _getch();
  int32_t         _kbhit();
  int32_t         fopen_s(FILE **,
                          const char *,
                          const char *);
  PICO_STATUS     changePowerSource(int16_t,
                                    PICO_STATUS);
  static void     callBackStreaming(int16_t,
                                    int32_t,
                                    uint32_t,
                                    int16_t,
                                    uint32_t,
                                    int16_t,
                                    int16_t,
                                    void *);
  void            streamDataHandler(UNIT *);
  void            setDefaults(UNIT *);
  void            collectStreamingImmediate(UNIT *);
  void            get_info(UNIT *);
  PICO_STATUS     openDevice(UNIT *);
  void            closeDevice(UNIT *);
  void            displaySettings(UNIT *);
  void            setVoltages(UNIT *, int);
  void            run();

public:
  std::queue<std::vector<int>>          xyLine;
  int16_t                               xyLineSize;
};


#endif //PLOT_HPP
