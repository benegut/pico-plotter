#include "window.hpp"
#include <string>





Buttons::Buttons()
{
  for(int i = 0; i<11; i++)
    {
      std::string name = Volt_Buttons_Names[i];
      Volt_Buttons.push_back(new QRadioButton(tr(name.c_str())));
    }

  for(int i = 0; i<4; i++)
    {
      std::string name = Mode_Buttons_Names[i];
      Mode_Buttons.push_back(new QRadioButton(tr(name.c_str())));
    }
}





VoltageButtonGroupbox::VoltageButtonGroupbox()
{
  Group_Layout = new QGridLayout();

  Buttons_Obj = new Buttons();
  for(auto i: Buttons_Obj->Volt_Buttons)
    Group_Layout->addWidget(i);

  this->setLayout(Group_Layout);
}





ModeButtonGroupbox::ModeButtonGroupbox()
{
  Group_Layout = new QGridLayout();

  Buttons_Obj = new Buttons();
  for(auto i: Buttons_Obj->Mode_Buttons)
    Group_Layout->addWidget(i);

  this->setLayout(Group_Layout);
}





XYZ_PicoChannelWindow::XYZ_PicoChannelWindow(UNIT * u)
  : unit(u)
{
  layout = new QGridLayout(this);


  for(int i = 0; i<PS3000A_MAX_CHANNELS; i++)
    {
      Enabled_CheckBox_Obj[i] = new QCheckBox(tr("&Enable"));
      layout->addWidget(Enabled_CheckBox_Obj[i], 0, i);
    }


  for(int i = 0; i<PS3000A_MAX_CHANNELS; i++)
    {
      VoltageButtonGroupbox_Obj[i] = new VoltageButtonGroupbox();
      layout->addWidget(VoltageButtonGroupbox_Obj[i], 1, i);
    }


  for(int i = 0; i<PS3000A_MAX_CHANNELS; i++)
    {
      Offset_SpinBox_Obj[i] = new QDoubleSpinBox();
      layout->addWidget(Offset_SpinBox_Obj[i], 2, i);
    }


  for(int i = 0; i<PS3000A_MAX_CHANNELS; i++)
    {
      ModeButtonGroupbox_Obj[i] = new ModeButtonGroupbox();
      layout->addWidget(ModeButtonGroupbox_Obj[i], 3, i);
    }


  Update_Button = new QPushButton(tr("&Update"));
  layout->addWidget(Update_Button, 4, 3);

  setLayout(layout);

  connect(Update_Button, SIGNAL(clicked()), this, SLOT(Update_Button_Slot()));
}





void XYZ_PicoChannelWindow::Update_Button_Slot()
{
  /*** Enable channel ****/
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    {
      if(Enabled_CheckBox_Obj[ch]->isChecked())
        unit->channelSettings[ch].enabled = 1;
      else
        {
          unit->channelSettings[ch].enabled = 0;
          unit->channelSettings[ch].range = PS3000A_MAX_RANGES - 2;
          unit->channelSettings[ch].xymode = OFF;
          unit->channelSettings[ch].offset = 0.0;
        }
    }


  /**** Set voltage range ****/
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    {
      for(int i=0; i<11; i++ )
        if(VoltageButtonGroupbox_Obj[ch]->Buttons_Obj->Volt_Buttons[i]->isChecked() &&
           Enabled_CheckBox_Obj[ch]->isChecked())
          unit->channelSettings[ch].range = i;
    }


  /**** Set offset spinbox ****/
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    {
      double dum = Offset_SpinBox_Obj[ch]->value();
      if(unit->channelSettings[ch].enabled &&
         dum <= unit->channelSettings[ch].maxOffset &&
         dum >= unit->channelSettings[ch].minOffset)
        unit->channelSettings[ch].offset = (float)dum;
      else if(!unit->channelSettings[ch].enabled)
        unit->channelSettings[ch].offset = 0.0;
      else if(unit->channelSettings[ch].enabled &&
              dum > unit->channelSettings[ch].maxOffset)
        unit->channelSettings[ch].offset = unit->channelSettings[ch].maxOffset;
      else if(unit->channelSettings[ch].enabled &&
              dum < unit->channelSettings[ch].minOffset)
        unit->channelSettings[ch].offset = unit->channelSettings[ch].minOffset;
    }


  /*** Set mode ****/
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    for(int i=0; i<4; i++ )
      if(ModeButtonGroupbox_Obj[ch]->Buttons_Obj->Mode_Buttons[i]->isChecked() &&
         Enabled_CheckBox_Obj[ch]->isChecked())
        unit->channelSettings[ch].xymode = (XYMODE)i;


  emit(stop_Stream_Signal());
  emit(send_Unit_Data_Signal(*unit));
  emit(update_XY_Axis());
  Update_Window();
  emit(start_Stream_Signal());
}





void XYZ_PicoChannelWindow::Update_Window()
{
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    if(unit->channelSettings[ch].enabled)
      Enabled_CheckBox_Obj[ch]->setChecked(1);


  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    for(int i=0; i<11; i++ )
      if(unit->channelSettings[ch].range == i)
        VoltageButtonGroupbox_Obj[ch]->Buttons_Obj->Volt_Buttons[i]->click();


  Get_Offset_Range();
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    Offset_SpinBox_Obj[ch]->setValue(unit->channelSettings[ch].offset);


  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    for(int i=0; i<4; i++ )
      if(unit->channelSettings[ch].xymode == (XYMODE)i)
        ModeButtonGroupbox_Obj[ch]->Buttons_Obj->Mode_Buttons[i]->click();
}





void XYZ_PicoChannelWindow::Get_Offset_Range()
{
  for(int ch = 0; ch < PS3000A_MAX_CHANNELS; ch++)
    {
      int dum = unit->channelSettings[ch].range;
      unit->channelSettings[ch].maxOffset = maxOffsetArray[dum];
      unit->channelSettings[ch].minOffset = minOffsetArray[dum];
      Offset_SpinBox_Obj[ch]->setMaximum(maxOffsetArray[dum]);
      Offset_SpinBox_Obj[ch]->setMinimum(minOffsetArray[dum]);
    }
}
