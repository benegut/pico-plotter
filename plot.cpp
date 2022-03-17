#include <QApplication>
#include "plot.hpp"
#include <iostream>
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


int32_t Plot::fopen_s(FILE ** a, const char * b, const char * c)
{
  FILE * fp = fopen(b,c);
  *a = fp;
  return (fp!=0)?0:-1;
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
void Plot::streamDataHandler(UNIT * unit)
{
    int16_t autostop = 0;
    int16_t retry = 0;
    int16_t powerChange = 0;

    int32_t totalSamples;
    int totalIndex = 0;

    uint32_t preTrigger = 0;
    uint32_t postTrigger = 100;
    uint32_t sampleCount = 10000; /* Make sure buffer large enough */
    uint32_t sampleInterval = 100;
    uint32_t downsampleRatio = 1;

    int16_t * buffer[PS3000A_MAX_CHANNEL_BUFFERS];
    int16_t * appBuffer[PS3000A_MAX_CHANNEL_BUFFERS];

    PICO_STATUS status;

    BUFFER_INFO bufferInfo;


    if(g_mode == 0)
      emit(resetPlot(unit));

    std::cout << "check_entered_streamer" << std::endl;

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
    }
    while(retry);

    printf("Streaming data...Press a key to stop\n");


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

          if(g_mode == 0)
            {
              QVector<double> key(g_sampleCount);
              std::iota(key.begin(), key.end(), totalSamples);

              totalSamples += g_sampleCount;

              if(totalSamples >= unit->timeBaseStart + unit->timeBase)
                emit(changeAxis(unit));

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

          else if(g_mode == 1)
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

          else if(g_mode == 2)
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

      if(_kbhit())
        {
          char ch;
          std::cout << "Exit Stream? [Y/n]" << std::endl;
          ch = _getch();
          ch = toupper(ch);
          if(ch == '\n' || ch == 'Y')
            {              std::cout << _kbhit() << std::endl;

           goto exit;
            }
          else
            continue;
        }
    }
 while (!_kbhit() && !g_autoStopped);

 exit:
    ps3000aStop(unit->handle);

    if (!g_autoStopped && !powerChange)
    {
        printf("\nData collection aborted.\n");
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
void Plot::setDefaults(UNIT * unit)
{
  PICO_STATUS status;

  for (int i = 0; i < unit->channelCount; i++) // reset channels to most recent settings
    {
      status = ps3000aSetChannel(unit->handle,
                                 (PS3000A_CHANNEL)(PS3000A_CHANNEL_A + i),
                                 unit->channelSettings[PS3000A_CHANNEL_A + i].enabled,
                                 (PS3000A_COUPLING)unit->channelSettings[PS3000A_CHANNEL_A + i].DCcoupled,
                                 (PS3000A_RANGE)unit->channelSettings[PS3000A_CHANNEL_A + i].range,
                                 unit->channelSettings[PS3000A_CHANNEL_A + i].offset);

      printf(status?"SetDefaults:ps3000aSetChannel------ 0x%08lx \n":"", (long unsigned int)status);

    }
}


/****************************************************************************
 * collectStreamingImmediate
 *  this function demonstrates how to collect a stream of data
 *  from the unit (start collecting immediately)
 ***************************************************************************/
void Plot::collectStreamingImmediate(UNIT * unit)
{
  setDefaults(unit);

  printf("Collect streaming...\n");
  //printf("Data is written to disk file (stream.txt)\n");

  streamDataHandler(unit);
}


/****************************************************************************
 * Initialise unit' structure with Variant specific defaults
 ****************************************************************************/
void Plot::get_info(UNIT * unit)
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
PICO_STATUS Plot::openDevice(UNIT * unit)
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
      unit->channelSettings[i].xymode = OFF;
    }

  setDefaults(unit);

  return status;
}


/****************************************************************************
 * closeDevice
 ****************************************************************************/
void Plot::closeDevice(UNIT *unit)
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
void Plot::displaySettings(UNIT *unit)
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
                printf("%dmV", voltage);
            }
            else
            {
                printf("%dV", voltage / 1000);
            }
            printf("           Mode = %s\n", xymode_txt[unit->channelSettings[ch].xymode]);
        }
    }
    printf("\n");
}


/****************************************************************************
* setVoltages
* Select input voltage ranges for channels
****************************************************************************/
void Plot::setVoltages(UNIT * unit, int mode)
{
    int32_t count = 0;
    bool    xBool = false;
    bool    yBool = false;
    bool    zBool = false;
    int     code = 0;

    /* See what ranges are available... */
    printf("Ranges:\n");
    for (int32_t i = unit->firstRange; i <= unit->lastRange; i++)
    {
        printf("%d -> %d mV\n", i, inputRanges[i]);
    }
    /* See what modes are available... */
    printf("\nModes:\n");
    for(int i=0; i<4; i++)
      printf("%d -> %s\n", i, xymode_txt[i]);

    printf("\n");


    do
    {
        /* Ask the user to select a range */
        printf("Specify voltage range (%d..%d) and mode (0..3)\n", unit->firstRange, unit->lastRange);
        printf("99 - switches channel off\n");

        int16_t graph = 0;

        for (int32_t ch = 0; ch < unit->channelCount; ch++)
        {
            printf("\n");

            do
            {
                printf("Channel %c range: ", 'A' + ch);
                fflush(stdin);
                scanf("%hd", &unit->channelSettings[ch].range);
            } while (unit->channelSettings[ch].range != 99 &&
                     (unit->channelSettings[ch].range < unit->firstRange ||
                      unit->channelSettings[ch].range > unit->lastRange));

            if (unit->channelSettings[ch].range != 99)
            {
                printf(" - %d mV\n", inputRanges[unit->channelSettings[ch].range]);
                unit->channelSettings[ch].enabled = 1;
                unit->channelSettings[ch].graph = graph;
                unit->channelSettings[ch].offset = 0.0;
                graph++;
                count++;
            }
            else
            {
                printf("Channel Switched off\n");
                unit->channelSettings[ch].enabled = 0;
                unit->channelSettings[ch].range = PS3000A_MAX_RANGES-1;
            }

            if(unit->channelSettings[ch].enabled)
              {
                printf("Offset channel %c: ", 'A' + ch);
                fflush(stdin);
                scanf("%f", &unit->channelSettings[ch].offset);
              }

            if((mode == 1) && unit->channelSettings[ch].enabled)
              {
                do
                  {
                    printf("Channel %c mode: ", 'A' + ch);
                    fflush(stdin);
                    scanf("%d", &unit->channelSettings[ch].xymode);
                    if(unit->channelSettings[ch].xymode >= 3)
                      printf("\nInvalide XY-Mode.\n");
                    else
                      {
                        printf(" - %s\n", xymode_txt[unit->channelSettings[ch].xymode]);
                        //code += unit->channelSettings[ch].xymode;
                      }
                  }
                while(unit->channelSettings[ch].xymode >= 3);
              }

            else if((mode == 2) && unit->channelSettings[ch].enabled)
              {
                do
                  {
                    printf("Channel %c mode: ", 'A' + ch);
                    fflush(stdin);
                    scanf("%d", &unit->channelSettings[ch].xymode);
                    if(unit->channelSettings[ch].xymode >= 4)
                      printf("\nInvalide XYZ-Mode.\n");
                    printf(" - %s\n", xymode_txt[unit->channelSettings[ch].xymode]);
                  }
                while(unit->channelSettings[ch].xymode >= 4);
              }
        }
        printf(count == 0? "\n** At least 1 channel must be enabled **\n\n":"");
        //printf(code == 0? "\n** Not enough XY modes selected **\n\n":"");

    }
    while(count == 0);	// must have at least one channel enabled

    if(mode == 0)
      {
        printf("\n");
        printf("Set time base (x1000): ");
        int t;
        scanf("%d", &t);
        unit->timeBase = t*1000;
        unit->timeBaseStart = 0;
        unit->timeBaseEnd = unit->timeBase;
      }

    setDefaults(unit);	// Put these changes into effect
}


/****************************************************************************************
 *run()
 ****************************************************************************************
 */
void Plot::run(){


  UNIT unit;
  PICO_STATUS status;

  status = openDevice(&unit);

  char ch = '.';
  int mode = 3;

  while(ch != 'X')
    {
      while(mode>2)
        {

          printf("\n");
          printf("0 -> Function\n");
          printf("1 -> XY-Mode\n");
          printf("2 -> XYZ-Mode\n");
          printf("\n\nChoose Mode: ");
          ch = _getch();
          mode = atoi(&ch);
        }

      g_mode = mode;

    menu:
      printf("\n\n");

      displaySettings(&unit);

      printf("\n\n");
      printf("Please select one of the following options:\n\n");
      printf("S - Immediate streaming                     V - Set voltages\n");
      printf("                                            X - Exit\n");
      printf("Operation:");

      ch = _getch();
      ch = toupper(ch);

      printf("\n");

      switch (ch)
        {
        case 'S':
          collectStreamingImmediate(&unit);
          break;

        case 'V':
          setVoltages(&unit, mode);
          if(mode == 0)
            emit(setNormalMode(&unit));
          else if(mode == 1)
            emit(setXYMode(&unit));
          else
            emit(setXYZMode(&unit));
          goto menu;

        case 'X':
          break;

        default:
          printf("Invalid operation\n");
          break;
        }

    }
  closeDevice(&unit);
  printf("\nDevice closed. Exit.\n");
  QApplication::instance()->quit();
}
