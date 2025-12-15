#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <../CM7/Core/Inc/fdcan.h>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void update_value(disp_data_struct disp_val);
    virtual void update_value_tick(uint16_t);
protected:
    int tickCounter;
	int analogHours;
	int analogMinutes;
	int analogSeconds;
};

#endif // SCREEN1VIEW_HPP
