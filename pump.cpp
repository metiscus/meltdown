#include "pump.h"

#include <cstdlib>
#include <iostream>

Pump::Pump()
	: max_flow_rate_(10000.0)
	, flow_rate_(0.0)
	, mean_time_to_fail_(3000.0)
	, powered_on_time_(0.0)
	, is_powered_(false)
	, is_failed_(false)
{

}

void Pump::update(float dt)
{
	powered_on_time_ += dt;
	if(powered_on_time_ > mean_time_to_fail_)
	{
		if((rand() % 1000) < 1)
		{
			is_failed_ = true;
		}
	}
}

float Pump::get_flow_rate() const
{
	float outflow = flow_rate_;
	if(outflow > max_flow_rate_)
	{
		outflow = max_flow_rate_;
	}
	if(!is_powered_ || is_failed_)
	{
		outflow = 0.0f;
	}

	return outflow;
}