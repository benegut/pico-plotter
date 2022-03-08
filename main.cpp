#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
//#include <curses.h>
#include <iostream>

#include <libps3000a-1.1/ps3000aApi.h>
#ifndef PICO_STATUS
#include <libps3000a-1.1/PicoStatus.h>
#endif

#include "qcustomplot.h"
#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define memcpy_s(a,b,c,d) memcpy(a,c,d)

#define PREF4 __stdcall

int32_t _kbhit()
{
  struct termios oldt, newt;
  int32_t bytesWaiting;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  setbuf(stdin, NULL);
  ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return bytesWaiting;
}


int32_t fopen_s(FILE ** a, const char * b, const char * c)
{
  FILE * fp = fopen(b,c);
  *a = fp;
  return (fp!=0)?0:-1;
}


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


int32_t adc_to_mv(int32_t raw, int32_t ch, UNIT * unit)
{
  return (raw * inputRanges[ch]) / unit->maxValue;
}


/****************************************************************************************
 * changePowerSource - function to handle switches between +5 V supply, and USB-only power
 * Only applies to PicoScope 34xxA/B/D/D MSO units
 ******************************************************************************************/
PICO_STATUS changePowerSource(int16_t handle, PICO_STATUS status)
{
    switch (status)
    {
        case PICO_POWER_SUPPLY_NOT_CONNECTED:

            printf("\n5 V power supply not connected");
            printf("\nPowering the unit via USB\n");
            status = ps3000aChangePowerSource(handle, PICO_POWER_SUPPLY_NOT_CONNECTED);
            if(status == PICO_POWER_SUPPLY_UNDERVOLTAGE)
                {
                  status = changePowerSource(handle, status);
                }
            break;

        case PICO_POWER_SUPPLY_CONNECTED:

            printf("\nUsing +5V power supply voltage\n");
            status = ps3000aChangePowerSource(handle, PICO_POWER_SUPPLY_CONNECTED);
            break;

        case PICO_POWER_SUPPLY_UNDERVOLTAGE:

                printf("\nUSB not supplying required voltage");
                printf("\nPlease plug in the +5 V power supply\n");
                exit(0);
                break;

    }
    return status;
}


/****************************************************************************
* Streaming callback
* Used by ps3000a data streaming collection calls, on receipt of data.
* Used to set global flags etc. checked by user routines
****************************************************************************/
void PREF4 callBackStreaming(
    int16_t     handle,
    int32_t		noOfSamples,
    uint32_t	startIndex,
    int16_t		overflow,
    uint32_t	triggerAt,
    int16_t		triggered,
    int16_t		autoStop,
    void		*pParameter)
{
    int32_t channel;
    BUFFER_INFO * bufferInfo = NULL;

    if (pParameter != NULL)
    {
        bufferInfo = (BUFFER_INFO *) pParameter;
    }

    // used for streaming
    g_sampleCount	= noOfSamples;
    g_startIndex	= startIndex;
    g_autoStopped	= autoStop;

    // flag to say done reading data
    g_ready = 1;

    if (bufferInfo != NULL && noOfSamples)
      {
        for (channel = 0; channel < bufferInfo->unit->channelCount; channel++)
          {
            if (bufferInfo->unit->channelSettings[channel].enabled)
              {
                if (bufferInfo->appBuffers && bufferInfo->driverBuffers)
                  {
                    if (bufferInfo->appBuffers[channel]  && bufferInfo->driverBuffers[channel])
                      {
                        memcpy_s (&bufferInfo->appBuffers[channel][startIndex], noOfSamples * sizeof(int16_t),
                                  &bufferInfo->driverBuffers[channel][startIndex], noOfSamples * sizeof(int16_t));
                      }
                    if (bufferInfo->appBuffers[channel] && bufferInfo->driverBuffers[channel])
                      {
                        memcpy_s (&bufferInfo->appBuffers[channel][startIndex], noOfSamples * sizeof(int16_t),
                                  &bufferInfo->driverBuffers[channel][startIndex], noOfSamples * sizeof(int16_t));
                      }
                  }
              }
          }
      }
}


/****************************************************************************
* Stream Data Handler
* - Used by the two stream data examples - untriggered and triggered
* Inputs:
* - unit - the unit to sample on
* - preTrigger - the number of samples in the pre-trigger phase
*					(0 if no trigger has been set)
***************************************************************************/
void streamDataHandler(UNIT * unit, int argc, char ** argv)
{
    int16_t autostop = 0;
    int16_t retry = 0;
    int16_t powerChange = 0;

    int32_t index = 0;
    int32_t totalSamples;

    uint32_t preTrigger = 0;
    uint32_t postTrigger = 100;
    uint32_t sampleCount = 10000; /* Make sure buffer large enough */
    uint32_t sampleInterval = 100;
    uint32_t downsampleRatio = 1;

    int16_t * buffer[PS3000A_MAX_CHANNEL_BUFFERS];
    int16_t * appBuffer[PS3000A_MAX_CHANNEL_BUFFERS];

    PICO_STATUS status;

    BUFFER_INFO bufferInfo;
    FILE * fp = NULL;

    for (int i = 0; i < unit->channelCount; i++)
      {
        if(unit->channelSettings[i].enabled)
          {
            buffer[i] = (int16_t*) calloc(sampleCount, sizeof(int16_t));

            status = ps3000aSetDataBuffer(unit->handle,
                                           (PS3000A_CHANNEL)i,
                                           buffer[i],
                                           sampleCount,
                                           0,
                                           PS3000A_RATIO_MODE_NONE);

            appBuffer[i] = (int16_t*) calloc(sampleCount, sizeof(int16_t));

            printf(status ? "StreamDataHandler:ps3000aSetDataBuffers(channel %d) ------ 0x%08lx \n":"",
                   i,
                   (long unsigned int)status);
          }
      }

    bufferInfo.unit = unit;
    bufferInfo.mode = ANALOGUE;
    bufferInfo.driverBuffers = buffer;
    bufferInfo.appBuffers = appBuffer;

    printf("\nStreaming Data for %u samples.\n", postTrigger / downsampleRatio);

    //printf("\nStreaming Data continually.\n\n");


    g_autoStopped = 0;

    do
    {
        retry = 0;
        status = ps3000aRunStreaming(unit->handle,
                                     &sampleInterval,
                                     PS3000A_US,
                                     preTrigger,
                                     postTrigger,
                                     autostop,
                                     downsampleRatio,
                                     PS3000A_RATIO_MODE_NONE,
                                     sampleCount);


        if(status != PICO_OK)
        {
            if(status == PICO_POWER_SUPPLY_CONNECTED ||
               status == PICO_POWER_SUPPLY_NOT_CONNECTED ||
               status == PICO_POWER_SUPPLY_UNDERVOLTAGE)
            {
                status = changePowerSource(unit->handle, status);
                retry = 1;
            }
            else
            {
              printf("StreamDataHandler:ps3000aRunStreaming ------ 0x%08lx \n", (long unsigned int)status);
              return;
            }
        }
    }
    while(retry);

    printf("Streaming data...Press a key to stop\n");

    fopen_s(&fp, StreamFile, "w");

    if (fp != NULL)
      {
        for (int i = 0; i < unit->channelCount; i++)
          {
            if (unit->channelSettings[i].enabled)
              {
                fprintf(fp,"&c\t", (char)('A' + i));
              }
          }
        fprintf(fp, "\n");
      }

    totalSamples = 0;

    //    QApplication::instance();
    // QApplication a(argc, (char **)argv);
    // QMainWindow window;
    // QCustomPlot customPlot;
    // window.setCentralWidget(&customPlot);

    // customPlot.addGraph();
    // customPlot.graph(0)->setPen(QPen(QColor(40, 110, 255)));

    // customPlot.rescaleAxes();
    // window.setGeometry(100, 100, 500, 400);
    // window.show();
    //a.exec();

    while (!_kbhit() && !g_autoStopped)
    {
        // Register callback function with driver and check if data has been received
        g_ready = 0;

        status = ps3000aGetStreamingLatestValues(unit->handle,
                                                 callBackStreaming,
                                                 &bufferInfo);

        if (status == PICO_POWER_SUPPLY_CONNECTED ||
            status == PICO_POWER_SUPPLY_NOT_CONNECTED ||
            status == PICO_POWER_SUPPLY_UNDERVOLTAGE)
        {
            changePowerSource(unit->handle, status);
            printf("\n\nPower Source Change");
            powerChange = 1;
        }

        index ++;

        if (g_ready && g_sampleCount > 0) /* can be ready and have no data, if autoStop has fired */
        {
            totalSamples += g_sampleCount;

            printf("\nCollected %i samples, index = %u, Total: %d samples ", g_sampleCount, g_startIndex, totalSamples);

            for (int i = g_startIndex; i < (int32_t)(g_startIndex + g_sampleCount); i++)
            {
                if(fp != NULL)
                {
                  for (int j = 0; j < unit->channelCount; j++)
                    {
                      if (unit->channelSettings[j].enabled)
                        {
                          fprintf(	fp,
                                    "%+d\t",
                                    adc_to_mv(appBuffer[j][i],
                                              unit->channelSettings[PS3000A_CHANNEL_A + j].range,
                                              unit)
                                    );
                        }
                    }
                  fprintf(fp, "\n");
                }
                else
                  {
                    printf("Cannot open the file %s for writing.\n", StreamFile);
                  }
            }
        }
    }

    ps3000aStop(unit->handle);

    if (!g_autoStopped && !powerChange)
    {
        printf("\nData collection aborted.\n");
    }

    if(fp != NULL)
    {
        fclose(fp);
    }

    for (int i = 0; i < unit->channelCount; i++)
      {
        if(unit->channelSettings[i].enabled)
          {
            free(buffer[i]);
            free(appBuffer[i]);
          }
      }
}


/****************************************************************************
 * setDefaults - restore default settings
 ****************************************************************************/
void setDefaults(UNIT * unit)
{
  PICO_STATUS status;

  for (int i = 0; i < unit->channelCount; i++) // reset channels to most recent settings
    {
      status = ps3000aSetChannel(unit->handle,
                                 (PS3000A_CHANNEL)(PS3000A_CHANNEL_A + i),
                                 unit->channelSettings[PS3000A_CHANNEL_A + i].enabled,
                                 (PS3000A_COUPLING)unit->channelSettings[PS3000A_CHANNEL_A + i].DCcoupled,
                                 (PS3000A_RANGE)unit->channelSettings[PS3000A_CHANNEL_A + i].range, 0);

      printf(status?"SetDefaults:ps3000aSetChannel------ 0x%08lx \n":"", (long unsigned int)status);
    }
}


/****************************************************************************
 * collectStreamingImmediate
 *  this function demonstrates how to collect a stream of data
 *  from the unit (start collecting immediately)
 ***************************************************************************/
void collectStreamingImmediate(UNIT * unit, int argc, char **argv)
{
  setDefaults(unit);

  printf("Collect streaming...\n");
  printf("Data is written to disk file (stream.txt)\n");

  streamDataHandler(unit, argc, argv);
}


/****************************************************************************
 * Initialise unit' structure with Variant specific defaults
 ****************************************************************************/
void get_info(UNIT * unit)
{
    char description [11][25]= { "Driver Version",
        "USB Version",
        "Hardware Version",
        "Variant Info",
        "Serial",
        "Cal Date",
        "Kernel",
        "Digital H/W",
        "Analogue H/W",
        "Firmware 1",
        "Firmware 2"};

    int8_t line[80];

    PICO_STATUS status = PICO_OK;

    //Initialise default unit properties and change when required
    unit->firstRange	= PS3000A_10MV;
    unit->lastRange		= PS3000A_20V;
    unit->channelCount	= 4;
    int16_t value;
    ps3000aMaximumValue(unit->handle, &value);
    unit->maxValue = value;

    if (unit->handle)
    {
        int16_t r;
        for (int i = 0; i < 11; i++)
        {
            status = ps3000aGetUnitInfo(unit->handle,
                                        line,
                                        sizeof(line),
                                        &r,
                                        i);

            if (i == 3)
            {
                memcpy(unit->model, line, strlen((char*)(line))+1);
            }
            printf("%s: %s\n", description[i], line);
        }
    }

    if(status == PICO_INVALID_HANDLE ||
       status == PICO_INVALID_INFO ||
       status == PICO_INFO_UNAVAILABLE)
      {
        printf("\n\nError in get_info()!\n");
      }
    if(status != PICO_OK)
      {
        printf("\n\nStatus of Picoscope smth else than ok. Check get_info().\n");
      }
}


/****************************************************************************
 * openDevice
 * Parameters
 * - unit        pointer to the UNIT structure, where the handle will be stored
 *
 * Returns
 * - PICO_STATUS to indicate success, or if an error occurred
 ***************************************************************************/
PICO_STATUS openDevice(UNIT * unit)
{
  PICO_STATUS status = ps3000aOpenUnit(&(unit->handle), NULL);

  if (status == PICO_POWER_SUPPLY_NOT_CONNECTED)
      status = changePowerSource(unit->handle, status);

  if (status != PICO_OK)
    {
      printf("Unable to open device\n");
      printf("Error code : 0x%08lx\n", (long unsigned int)status);
      exit(99);
    }

  printf("Device opened successfully, \n\n");

  get_info(unit);

  for (int i = 0; i < unit->channelCount; i++)
    {
      unit->channelSettings[i].enabled = 1;
      unit->channelSettings[i].DCcoupled = 1;
      unit->channelSettings[i].range = PS3000A_5V;
    }

  setDefaults(unit);

  return status;
}


/****************************************************************************
 * closeDevice
 ****************************************************************************/
void closeDevice(UNIT *unit)
{
  ps3000aCloseUnit(unit->handle);

}


/****************************************************************************
* displaySettings
* Displays information about the user configurable settings in this example
* Parameters
* - unit        pointer to the UNIT structure
*
* Returns       none
***************************************************************************/
void displaySettings(UNIT *unit)
{
    int32_t voltage;

    for (char ch = 0; ch < unit->channelCount; ch++)
    {
        if (!(unit->channelSettings[ch].enabled))
        {
            printf("Channel %c Voltage Range = Off\n", 'A' + ch);
        }
        else
        {
            voltage = inputRanges[unit->channelSettings[ch].range];
            printf("Channel %c Voltage Range = ", 'A' + ch);

            if (voltage < 1000)
            {
                printf("%dmV\n", voltage);
            }
            else
            {
                printf("%dV\n", voltage / 1000);
            }
        }
    }
    printf("\n");
}


/****************************************************************************
* setVoltages
* Select input voltage ranges for channels
****************************************************************************/
void setVoltages(UNIT * unit)
{
    int32_t count = 0;

    /* See what ranges are available... */
    for (int32_t i = unit->firstRange; i <= unit->lastRange; i++)
    {
        printf("%d -> %d mV\n", i, inputRanges[i]);
    }

    do
    {
        /* Ask the user to select a range */
        printf("Specify voltage range (%d..%d)\n", unit->firstRange, unit->lastRange);
        printf("99 - switches channel off\n");

        for (int32_t ch = 0; ch < unit->channelCount; ch++)
        {
            printf("\n");
            do
            {
                printf("Channel %c: ", 'A' + ch);
                fflush(stdin);
                scanf("%hd", &unit->channelSettings[ch].range);
            } while (unit->channelSettings[ch].range != 99 &&
                     (unit->channelSettings[ch].range < unit->firstRange ||
                      unit->channelSettings[ch].range > unit->lastRange));

            if (unit->channelSettings[ch].range != 99)
            {
                printf(" - %d mV\n", inputRanges[unit->channelSettings[ch].range]);
                unit->channelSettings[ch].enabled = 1;
                count++;
            }
            else
            {
                printf("Channel Switched off\n");
                unit->channelSettings[ch].enabled = 0;
                unit->channelSettings[ch].range = PS3000A_MAX_RANGES-1;
            }
        }
        printf(count == 0? "\n** At least 1 channel must be enabled **\n\n":"");
    }
    while(count == 0);	// must have at least one channel enabled

    setDefaults(unit);	// Put these changes into effect
}


/****************************************************************************************
 *main()
 ****************************************************************************************
 */
int main(int argc, char *argv[]){


  UNIT unit;
  PICO_STATUS status;

  status = openDevice(&unit);

  char ch = '.';

    while (ch != 'X')
    {
        displaySettings(&unit);

        printf("\n\n");
        printf("Please select one of the following options:\n\n");
        printf("S - Immediate streaming                     V - Set voltages\n");
        printf("                                            X - Exit\n");
        printf("Operation:");

        std::cin >> ch;
        ch = toupper(ch);

        printf("\n");

        switch (ch)
        {
            case 'S':
              collectStreamingImmediate(&unit, argc, argv);
                break;

            case 'V':
                setVoltages(&unit);
                break;

            case 'X':
                break;

            default:
                printf("Invalid operation\n");
                break;
        }
    }

    closeDevice(&unit);
    printf("\nDevice closed. Exit.\n");
    return 1;
}
