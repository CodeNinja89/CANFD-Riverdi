#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <../CM7/Core/Inc/fdcan.h>

#define UPDATE_FREQ_MS		1000
uint32_t tick_count = 0;
uint32_t mot_rpm = 0;

disp_data_struct disp_val;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	// memcpy(&disp_val, &disp_data_struct, sizeof(disp_data_struct));
	// update_value();
}

void Model::update_value()
{
	modelListener->update_value(disp_val);
}
