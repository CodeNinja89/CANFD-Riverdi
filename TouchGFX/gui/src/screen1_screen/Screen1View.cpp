#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View :: handleTickEvent()
{
	 tickCounter++;

	if (tickCounter % 60 == 0)
	{
		if (++analogSeconds >= 60)
		{
			analogSeconds = 0;
			if (++analogMinutes >= 60)
			{
				analogMinutes = 0;
				if (++analogHours >= 24)
				{
					analogHours = 0;
				}
			}
		}

		// Update the clocks
		analogClock.setTime24Hour(analogHours, analogMinutes, analogSeconds);
		//update_value_tick(analogSeconds);
	}
}

void Screen1View::update_value(disp_data_struct disp_val)
{
	Unicode::snprintf(textAreaMotRPMBuffer, 10, "%d", disp_val.mot_rpm);
	// Unicode::strncpy(textAreaMotRPM, "TouchGFX", 1200);
	textAreaMotRPM.invalidate();
}

void Screen1View::update_value_tick(uint16_t seconds)
{
	Unicode::snprintf(textAreaMotRPMBuffer, 10, "%d", seconds);
	// Unicode::strncpy(textAreaMotRPM, "TouchGFX", 1200);
	textAreaMotRPM.invalidate();
}
