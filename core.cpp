#include "core.h"
#include "physics.h"
#include <cassert>
#include <iostream>

Core::Core()
	: control_rods_(1.0f)
	, max_power_(1000e6f)
	//, temperature_coefficient_(0.002)
	, temperature_coefficient_(0.002)
	, boiling_(false)
	, fuel_cladding_melt_temp_(726.0)
	, fuel_cladding_integrity_(1.0)
	, reactor_vessel_(60000.0, 2600*Physics::PSIToKPa)
{
	double quantity = 10000.0;
	reactor_vessel_.add(quantity, 100.0);
}

void Core::set_control_rod_position(float pos)
{
	control_rods_ = pos;
	if(control_rods_ > 1.0f)
	{
		control_rods_ = 1.0;
	}
	else if(control_rods_ < 0.0f)
	{
		control_rods_ = 0.0f;
	}
}

void Core::set_max_power(float heat)
{
	max_power_ = heat;
}

void Core::update(float dt)
{
	double thermal_coefficient = reactor_vessel_.get_temperature() * temperature_coefficient_;
	// boiling water moderates the reaction very poorly, I model this by increasing the temperature coefficient
	// however, boiling water can absorb less heat (~50% by mass) so that needs modeled
	// TODO: boiling water should increase the system pressure (steam explosion)
	if(boiling_)
	{
		thermal_coefficient *= 1.1;
	}

	double energy_generated = dt * max_power_ * (1.0f - control_rods_) * (1.0 - thermal_coefficient);
	//std::cerr<<"Core generated "<<energy_generated<< "energy\n";
	//std::cerr<<"Pre xfer temp: " << reactor_vessel_.get_temperature() <<"\n";
	reactor_vessel_.transfer_thermal_energy( energy_generated );
	//std::cerr<<"Post xfer temp: " << reactor_vessel_.get_temperature() <<"\n";
	double boiling_point = Physics::compute_water_boil_temperature(reactor_vessel_.get_pressure());
	if(reactor_vessel_.get_temperature() > boiling_point)
	{
		std::cerr<<"The water in the reactor core is boiling!\n";
		boiling_ = true;
	}
	else
	{
		boiling_ = false;
	}
	std::cerr<<"boiling point is: "<<boiling_point<<" : temperature of coolant: " << reactor_vessel_.get_temperature() << "\n";

	// check for fuel cladding melt
	if(reactor_vessel_.get_temperature() > fuel_cladding_melt_temp_)
	{
		fuel_cladding_integrity_ -= (reactor_vessel_.get_temperature() - fuel_cladding_melt_temp_) / fuel_cladding_melt_temp_ * dt;
		std::cerr<<"The fuel cladding is melting! Integrity: "<<fuel_cladding_integrity_<<"\n";
		if(fuel_cladding_integrity_ < 0.0f)
		{
			fuel_cladding_integrity_ = 0.0f;
		}
	}
}

// quantity is in kg
void Core::add_coolant(double quantity, double temperature)
{
	assert(quantity >= 0);
	assert(temperature > 0);

	reactor_vessel_.add(quantity, temperature);
}

double Core::remove_coolant(double& quantity)
{
	assert(quantity >= 0);

	double temperature;
	double thermal_energy_taken = reactor_vessel_.take(quantity, temperature);
	return thermal_energy_taken;
}

const float Core::get_temperature() const
{
	return reactor_vessel_.get_temperature();
}