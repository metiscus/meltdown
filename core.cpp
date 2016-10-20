#include "core.h"
#include "physics.h"
#include <cassert>
#include <iostream>

Core::Core()
	: control_rods_(1.0f)
	, max_power_(1000e6f)
	, thermal_energy_(0.0f)
	, pressure_(2600.0f)
	, coolant_quantity_(10000.0f)
	//, temperature_coefficient_(0.002)
	, temperature_coefficient_(0.001)
	, boiling_(false)
	, fuel_cladding_melt_temp_(726.0)
	, fuel_cladding_integrity_(1.0)
{
	;
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
	double coolant_temperature = thermal_energy_ / (coolant_quantity_ * 1e3 * Constants::WaterSpecificHeat);
	double thermal_coefficient = coolant_temperature * temperature_coefficient_;
	// boiling water moderates the reaction very poorly, I model this by increasing the temperature coefficient
	// however, boiling water can absorb less heat (~50% by mass) so that needs modeled
	// TODO: boiling water should increase the system pressure (steam explosion)
	if(boiling_)
	{
		thermal_coefficient *= 1.1;
	}

	thermal_energy_ += dt * max_power_ * (1.0f - control_rods_) * (1.0 - thermal_coefficient);

	double boiling_point = Constants::compute_water_boil_temperature(pressure_);
	coolant_temperature = thermal_energy_ / (coolant_quantity_ * 1e3 * Constants::WaterSpecificHeat);
	if(coolant_temperature > boiling_point)
	{
		std::cerr<<"The water in the reactor core is boiling!\n";
		boiling_ = true;
	}
	else
	{
		boiling_ = false;
	}
	std::cerr<<"boiling point is: "<<boiling_point<<" : temperature of coolant: " << coolant_temperature << "\n";

	// check for fuel cladding melt
	if(coolant_temperature > fuel_cladding_melt_temp_)
	{
		fuel_cladding_integrity_ -= (coolant_temperature - fuel_cladding_melt_temp_) / fuel_cladding_melt_temp_ * dt;
		std::cerr<<"The fuel cladding is melting! Integrity: "<<fuel_cladding_integrity_<<"\n";
		if(fuel_cladding_integrity_ < 0.0f)
		{
			fuel_cladding_integrity_ = 0.0f;
		}
	}
}

// quantity is in kg
void Core::add_coolant(float quantity, float temperature)
{
	assert(quantity >= 0);
	assert(temperature > 0);

	thermal_energy_ += 1e3 * quantity * temperature * Constants::WaterSpecificHeat;
	coolant_quantity_ += quantity;
}

float Core::remove_coolant(float quantity)
{
	assert(quantity >= 0);

	if(quantity < 0.001) return (thermal_energy_ / (coolant_quantity_ * Constants::WaterSpecificHeat));

	double coolant_xfer_fraction = quantity / coolant_quantity_;
	if(boiling_)
	{
		coolant_xfer_fraction *= 0.5;
	}

	double energy_removed = (thermal_energy_ * coolant_xfer_fraction);
	thermal_energy_ -= energy_removed;

	if(!boiling_)
	{
		return (energy_removed / (quantity * 1e3)) / Constants::WaterSpecificHeat;
	}
	else
	{
		return (energy_removed / (quantity * 1e3)) / Constants::SteamSpecificHeat;
	}
}

const float Core::get_temperature() const
{
	return (thermal_energy_ / (coolant_quantity_ * 1e3)) / Constants::WaterSpecificHeat;
}