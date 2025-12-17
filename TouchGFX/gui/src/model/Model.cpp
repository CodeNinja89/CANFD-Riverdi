#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <../CM7/Core/Inc/fdcan.h>

#define UPDATE_FREQ_MS		1000
uint32_t tick_count = 0;
uint32_t mot_rpm = 0;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	// memcpy(&disp_val, &disp_data_struct, sizeof(disp_data_struct));
	// disp_val = disp_data;
//	tick_count++;
//	if(tick_count % 100 == 0) tick_count = 0;
//	modelListener->update_value(tick_count);

	uint32_t v;
	if (CANValue_Get(&v))
	{
		modelListener->update_value(v);   // we’ll add this hook
	}
}

void Model::update_value()
{
//	disp_val = disp_data;
//	modelListener->update_value(disp_val);
}
