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

#include "qcustomplot.h"
#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <cstdio>
#include <cstring>
#include <string>

#define memcpy_s(a, b, c, d) memcpy(a, c, d)

#define PREF4 __stdcall

typedef struct
{
  int16_t DCcoupled;
  int16_t range;
  int16_t enabled;
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
  int16_t					handle;
  int8_t					model[8];
  PS3000A_RANGE             firstRange ;
  PS3000A_RANGE             lastRange;
  int16_t					channelCount;
  int16_t					maxValue;
  CHANNEL_SETTINGS          channelSettings [PS3000A_MAX_CHANNELS];
}UNIT;


bool        g_ready = 0;
uint32_t	g_startIndex;
int16_t		g_autoStopped;
int16_t		g_trig = 0;
uint32_t	g_trigAt = 0;
int32_t     g_sampleCount;


uint16_t inputRanges [PS3000A_MAX_RANGES] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000};



char StreamFile[20]		= "stream.txt";

typedef enum
  {
    ANALOGUE,
    DIGITAL,
    AGGREGATED,
    MIXED
  }MODE;

typedef struct tBufferInfo
{
  UNIT * unit;
  MODE mode;
  int16_t **driverBuffers;
  int16_t **appBuffers;

} BUFFER_INFO;



class Plot
{
public:
  Plot();

private:
  int32_t     _kghit();
  int32_t     fopen_s(FILE **, const char *, const char *);
  int32_t     adc_to_mv(int32_t, int32_t, UNIT *);
  PICO_STATUS changePowerSource(int16_t, PICO_STATUS);
  void PREF4  callBackStreaming(int16_t,
                               int32_t,
                               uint32_t,
                               int16_t,
                               uint32_t,
                               int16_t,
                               int16_t,
                               void *);
  void        streamDataHandler(UNIT *, int, char **);
  void        setDefaults(UNIT *);
  void        collectStreamingImmediate(UNIT *, int, char **);
  void        get_info(UNIT *);
  PICO_STATUS openDevice(UNIT *);
  void        closeDevice(UNIT *);
  void        displaySettings(UNIT *);
  void        setVoltages(UNIT *);
  void        menu();
};



