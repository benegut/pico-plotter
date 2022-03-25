#include <QApplication>
#include "plot.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <QThread>





Worker::Worker()
{
  unit = new UNIT();

  PICO_STATUS status = ps3000aOpenUnit(&(unit->handle), NULL);

  if (status == PICO_POWER_SUPPLY_NOT_CONNECTED)
    status = changePowerSource(unit->handle, status);

  if (status != PICO_OK)
    printf("Unable to open device\n");

}





void Worker::start()
{
  get_info();
  setChannels();
  setDefaults();
  getOffsetArrays();

  emit(send_Unit_Data_Signal(*unit));
}




void Worker::get_info()
{
  unit->firstRange	 = PS3000A_10MV;
  unit->lastRange		 = PS3000A_20V;
  unit->channelCount = PS3000A_MAX_CHANNELS;
  ps3000aMaximumValue(unit->handle, &unit->maxValue);


  char description [11][25]= {
    "Driver Version",
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
  int16_t r;
  PICO_STATUS status;

  if (unit->handle)
    {
      for (int i = 0; i < 11; i++)
        {
          status = ps3000aGetUnitInfo(unit->handle,
                                      line,
                                      sizeof(line),
                                      &r,
                                      i);
          printf("%s: %s\n", description[i], line);
        }
    }

  if(status == PICO_INVALID_HANDLE ||
     status == PICO_INVALID_INFO ||
     status == PICO_INFO_UNAVAILABLE ||
     status != PICO_OK)
    printf("\n\nError in get_info()!\n");
}





void Worker::setChannels()
{

  for (int ch = 0; ch < unit->channelCount; ch++)
    {
      unit->channelSettings[ch].range         = PS3000A_5V;
      unit->channelSettings[ch].enabled       = true;
      unit->channelSettings[ch].bufferEnabled = false;
      unit->channelSettings[ch].xymode        = OFF;
      unit->channelSettings[ch].offset        = 0.0;
      unit->channelSettings[ch].maxOffset     = 0.0;
      unit->channelSettings[ch].minOffset     = 0.0;
    }
}





void Worker::setDefaults()
{
  PICO_STATUS status;

  for (int ch = 0; ch < unit->channelCount; ch++) // reset channels to most recent settings
    {
      status = ps3000aSetChannel(unit->handle,
                                 (PS3000A_CHANNEL)(PS3000A_CHANNEL_A + ch),
                                 unit->channelSettings[PS3000A_CHANNEL_A + ch].enabled,
                                 PS3000A_DC,
                                 (PS3000A_RANGE)unit->channelSettings[PS3000A_CHANNEL_A + ch].range,
                                 unit->channelSettings[PS3000A_CHANNEL_A + ch].offset);

      printf(status?"SetDefaults:ps3000aSetChannel------ 0x%08lx \n":"", (long unsigned int)status);

    }
}





void Worker::getOffsetArrays()
{
  for(int i = unit->firstRange; i <= unit->lastRange; i++)
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





void Worker::get_Unit_Data_Slot(UNIT u)
{
  *unit = u;
  setDefaults();
}





PICO_STATUS Worker::changePowerSource(int16_t handle, PICO_STATUS status)
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





void Worker::callBackStreaming(
                               int16_t     handle,
                               int32_t		noOfSamples,
                               uint32_t	startIndex,
                               int16_t		overflow,
                               uint32_t	triggerAt,
                               int16_t		triggered,
                               int16_t		autoStop,
                               void		*pParameter)
{
  BUFFER_INFO * bufferInfo = NULL;

  if (pParameter != NULL)
    bufferInfo = (BUFFER_INFO *) pParameter;

  g_sampleCount     = noOfSamples;
  g_startIndex      = startIndex;
  g_streamIsRunning	= !autoStop;

  // flag to say done reading data
  g_ready = true;

  if (bufferInfo != NULL && noOfSamples)
    {
      for (int ch = 0; ch < bufferInfo->unit->channelCount; ch++)
        {
          if (bufferInfo->unit->channelSettings[ch].bufferEnabled)
            {
              memcpy(&bufferInfo->appBuffers[ch][startIndex],
                     &bufferInfo->driverBuffers[ch][startIndex],
                     noOfSamples * sizeof(int16_t));
            }
        }
    }
}





void Worker::streamDataHandler()
{
  uint32_t    sampleCount = 10000; /* Make sure buffer large enough */
  uint32_t    sampleInterval = 100;

  int16_t *   buffer[PS3000A_MAX_CHANNEL_BUFFERS];
  int16_t *   appBuffer[PS3000A_MAX_CHANNEL_BUFFERS];
  BUFFER_INFO bufferInfo;


  for (int ch = 0; ch < unit->channelCount; ch++)
    {
      if(unit->channelSettings[ch].enabled)
        {
          buffer[ch] = (int16_t*) calloc(sampleCount, sizeof(int16_t));

          ps3000aSetDataBuffer(unit->handle,
                               (PS3000A_CHANNEL)ch,
                               buffer[ch],
                               sampleCount,
                               0,
                               PS3000A_RATIO_MODE_NONE);

          appBuffer[ch] = (int16_t*) calloc(sampleCount, sizeof(int16_t));
          unit->channelSettings[ch].bufferEnabled = true;
        }
    }

  bufferInfo.unit           = unit;
  bufferInfo.driverBuffers  = buffer;
  bufferInfo.appBuffers     = appBuffer;

  g_streamIsRunning = true;

  ps3000aRunStreaming(unit->handle,
                      &sampleInterval,
                      PS3000A_US,
                      0,//preTrigger
                      100,//postTrigger
                      0,//autostop
                      1,//downsampleRatio
                      PS3000A_RATIO_MODE_NONE,
                      sampleCount);

  do
    {
      g_ready = 0;

      ps3000aGetStreamingLatestValues(unit->handle,
                                      callBackStreaming,
                                      &bufferInfo);

      if(g_ready && g_sampleCount > 0)
        {
          double x,y,z,z2;
          for(int i = g_startIndex; i < (int32_t)(g_startIndex + g_sampleCount); i++)
            {
              for(int ch = 0; ch < unit->channelCount; ch++)
                {
                  int ind = unit->firstRange + unit->channelSettings[ch].range;
                  if(unit->channelSettings[ch].xymode == 1)
                    x = ((double)bufferInfo.appBuffers[ch][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                  else if(unit->channelSettings[ch].xymode == 2)
                    y = ((double)bufferInfo.appBuffers[ch][i]*((double)inputRanges[ind])/((double)unit->maxValue));
                  else if(unit->channelSettings[ch].xymode == 3)
                    z = ((double)bufferInfo.appBuffers[ch][i]/((double)unit->maxValue));
                  else if(unit->channelSettings[ch].xymode == 0)
                    z2 = ((double)bufferInfo.appBuffers[ch][i]/((double)unit->maxValue));
                }
              emit(sendData(x,y,z,z2));
            }
        }
    }
  while (g_streamIsRunning);


  ps3000aStop(unit->handle);


  for (int ch = 0; ch < unit->channelCount; ch++)
    {
      if(unit->channelSettings[ch].bufferEnabled)
        {
          free(buffer[ch]);
          free(appBuffer[ch]);
          unit->channelSettings[ch].bufferEnabled = 0;
        }
    }
}
