#include "pump.h"
#include "tank.h"
#include "physics.h"

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>

Pump::Pump(std::string name)
	: max_flow_rate_(10000.0)
	, flow_rate_(0.0)
	//, mean_time_to_fail_(3000.0)
	, mean_time_to_fail_(300.0)
	, powered_on_time_(0.0)
	, is_powered_(false)
	, is_failed_(false)
	, take_tank_(0)
	, add_tank_(0)
	, is_flawless_(true)
	, name_(name)
{

}

void Pump::update(float dt)
{
	powered_on_time_ += dt;
	if(powered_on_time_ > mean_time_to_fail_ && !is_flawless_)
	{
		if((rand() % 1000) < 1)
		{
			is_failed_ = true;
			return;
		}
	}

	if(is_failed_)
	{
	//	std::cerr<<"A PUMP HAS FAILED\n\n\n\n\n\nFAILED FAILED FAILED\n";
	}

	if(is_powered_ && !is_failed_)
	{
		if(take_tank_ && add_tank_)
		{
			double quantity = flow_rate_ / 60.0 * dt;
			double temperature = -1.0;
			double thermal_energy_removed = take_tank_->take(quantity, temperature);
			assert(quantity >= flow_rate_ / 60.0 * dt);

			//std::cerr<<"Pump: "<<name_<<" "<<add_tank_->get_maximum_capacity()<<" : "<<add_tank_->get_quantity()<<"\n";
			double added = add_tank_->add(quantity, temperature);
			if(fabs(quantity-added) > 0.01)
			{
				// receiving tank is overfull
				// put the fluid back into the first tank
				take_tank_->add(quantity - added, temperature);
			}

			if(added < flow_rate_ / 60.0 * dt)
			{
				std::cerr<<"Pump: "<<name_<<" has stalled. Could not pump. "<< added << "<" << flow_rate_ / 60.0 * dt << "\n";
			}
			assert(added >= flow_rate_ / 60.0 * dt);
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