#include <QApplication>
#include "plot.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <QThread>


int32_t Plot::_getch()
{
  struct termios oldt, newt;
  int32_t ch;
  int32_t bytesWaiting;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  setbuf(stdin, NULL);
  do {
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    if (bytesWaiting)
      getchar();
  } while (bytesWaiting);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}


int32_t Plot::_kbhit()
{
  struct termios oldt, newt;
  int32_t bytesWaiting = 0;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  setbuf(stdin, NULL);
  ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  //std::cout << bytesWaiting << std::endl;
  return bytesWaiting;
}



/****************************************************************************************
 * changePowerSource - function to handle switches between +5 V supply, and USB-only power
 * Only applies to PicoScope 34xxA/B/D/D MSO units
 ******************************************************************************************/
PICO_STATUS Plot::changePowerSource(int16_t handle, PICO_STATUS status)
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
void Plot::callBackStreaming(
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
void Plot::streamDataHandler(int mode)
{
    int16_t autostop = 0;
    int16_t retry = 0;

    int32_t totalSamples;
    int32_t totalIndex = 0;

    uint32_t preTrigger = 0;
    uint32_t postTrigger = 100;
    uint32_t sampleCount = 10000; /* Make sure buffer large enough */
    uint32_t sampleInterval = 100;
    uint32_t downsampleRatio = 1;

    int16_t * buffer[PS3000A_MAX_CHANNEL_BUFFERS];
    int16_t * appBuffer[PS3000A_MAX_CHANNEL_BUFFERS];

    PICO_STATUS status;

    BUFFER_INFO bufferInfo;

    setDefaults();


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

            unit->channelSettings[i].bufferEnabled = 1;

            printf(status ? "StreamDataHandler:ps3000aSetDataBuffers(channel %d) ------ 0x%08lx \n":"",
                   i,
                   (long unsigned int)status);
          }
      }

    bufferInfo.unit = unit;
    bufferInfo.driverBuffers = buffer;
    bufferInfo.appBuffers = appBuffer;


    g_autoStart = true;
    g_autoStopped = false;

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
    }
    while(retry);


    totalSamples = 0;

    do
      {
      // Register callback function with driver and check if data has been received
      g_ready = 0;

      status = ps3000aGetStreamingLatestValues(unit->handle,
                                               this->callBackStreaming,
                                               &bufferInfo);

      if(g_ready && g_sampleCount > 0)
        {

          if(mode == 0)
            {
              QVector<double> key(g_sampleCount);
              std::iota(key.begin(), key.end(), totalSamples);

              totalSamples += g_sampleCount;

              if(totalSamples >= unit->timeBaseStart + unit->timeBase)
                emit(changeAxis());

              QVector<QVector<double>> vec(g_sampleCount*8);

              for(int i = 0; i < bufferInfo.unit->channelCount; i++)
                {
                  if (bufferInfo.unit->channelSettings[i].enabled)
                    {
                      std::copy(&bufferInfo.appBuffers[i][g_startIndex],
                                &bufferInfo.appBuffers[i][g_startIndex+g_sampleCount],
                                std::back_inserter(vec[i]));
                      int ind = unit->firstRange + unit->channelSettings[i].range;
                      for(int j = 0; j < g_sampleCount; j++){vec[i][j] *= ((double)inputRanges[ind])/((double)unit->maxValue);}
                      emit (sendData(key, vec[i], unit->channelSettings[i].graph));
                    }
                }
            }

          else if(mode == 1)
            {
              totalSamples += g_sampleCount;
              double x,y;
              for(int i = g_startIndex; i < (int32_t)(g_startIndex + g_sampleCount); i++)
                {
                  for(int j = 0; j < unit->channelCount; j++)
                    {
                      int ind = unit->firstRange + unit->channelSettings[j].range;
                      if(unit->channelSettings[j].xymode == 1)
                        x = ((double)bufferInfo.appBuffers[j][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                      else if(unit->channelSettings[j].xymode == 2)
                        y = ((double)bufferInfo.appBuffers[j][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                    }
                  emit(sendData(x,y));
                }
            }

          else if(mode == 2)
            {
              totalSamples += g_sampleCount;

              double x,y,z;
              for(int i = g_startIndex; i < (int32_t)(g_startIndex + g_sampleCount); i++)
                {
                  for(int j = 0; j < unit->channelCount; j++)
                    {
                      int ind = unit->firstRange + unit->channelSettings[j].range;
                      if(unit->channelSettings[j].xymode == 1)
                        x = ((double)bufferInfo.appBuffers[j][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                        else if(unit->channelSettings[j].xymode == 2)
                        y = ((double)bufferInfo.appBuffers[j][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                      else if(unit->channelSettings[j].xymode == 3)
                        z = ((double)bufferInfo.appBuffers[j][i]/((double)unit->maxValue));
                    }
                  emit(sendData(x,y,z));
                }
            }
        }
      if(!g_autoStart)
        break;
    }
    while (!g_autoStopped);


    ps3000aStop(unit->handle);
    setDefaults();

    for (int i = 0; i < unit->channelCount; i++)
      {
        if(unit->channelSettings[i].bufferEnabled)
          {
            free(buffer[i]);
            free(appBuffer[i]);
            unit->channelSettings[i].bufferEnabled = 0;
          }
      }
}


/****************************************************************************
 * setDefaults - restore default settings
 ****************************************************************************/
void Plot::setDefaults()
{
  PICO_STATUS status;

  for (int i = 0; i < unit->channelCount; i++) // reset channels to most recent settings
    {
      status = ps3000aSetChannel(unit->handle,
                                 (PS3000A_CHANNEL)(PS3000A_CHANNEL_A + i),
                                 unit->channelSettings[PS3000A_CHANNEL_A + i].enabled,
                                 PS3000A_DC,
                                 (PS3000A_RANGE)unit->channelSettings[PS3000A_CHANNEL_A + i].range,
                                 unit->channelSettings[PS3000A_CHANNEL_A + i].offset);

      printf(status?"SetDefaults:ps3000aSetChannel------ 0x%08lx \n":"", (long unsigned int)status);

    }
}


/****************************************************************************
 * Initialise unit' structure with Variant specific defaults
 ****************************************************************************/
void Plot::get_info()
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
PICO_STATUS Plot::openDevice()
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

  get_info();
  setChannels();
  setDefaults();
  getOffsetArrays();

  return status;
}


/****************************************************************************
 * closeDevice
 ****************************************************************************/
void Plot::closeDevice()
{
  ps3000aCloseUnit(unit->handle);
}


void Plot::setChannels()
{

  for (int i = 0; i < unit->channelCount; i++)
    {
      unit->channelSettings[i].enabled = 1;
      unit->channelSettings[i].range = PS3000A_5V;
      unit->channelSettings[i].xymode = OFF;
      unit->channelSettings[i].offset = 0.0;
      unit->channelSettings[i].graph = i;
      unit->channelSettings[i].bufferEnabled = 0;
    }
}



void Plot::getOffsetArrays()
{
  for(int i= unit->firstRange; i<=unit->lastRange; i++)
    {
      float min, max;
      ps3000aGetAnalogueOffset(unit->handle,
                               (PS3000A_RANGE)i,
                               PS3000A_DC,
                               &max,
                               &min);
      min = min > -10e-4 ? 0.0 : min;
      max = max < 10e-4 ? 0.0 : max;
      minOffsetArray.push_back(min);
      maxOffsetArray.push_back(max);
    }
}



/****************************************************************************************
 *run()
 ****************************************************************************************
 */
void Plot::run(){

  g_ready = 0;
  g_autoStart = 1;

  unit = new UNIT();
  PICO_STATUS status;
  status = openDevice();

  unit->channelSettings[0].xymode = X;
  unit->channelSettings[1].xymode = Y;
  unit->channelSettings[2].xymode = Z;
  unit->channelSettings[3].xymode = OFF;
  unit->channelSettings[3].enabled = 0;

  emit(setXYZMode());

  while(true)
    {
      if(g_autoStart)
        streamDataHandler(2);

      fflush(stdin);
    }

  closeDevice();
  printf("\nDevice closed. Exit.\n");
  QApplication::instance()->quit();
}
