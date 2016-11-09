#include "tank.h"
#include "physics.h"

Tank::Tank(double maximum_capacity, double pressure)
	: quantity_(0)
	, temperature_(0)
	, thermal_energy_(0)
	, maximum_capacity_(maximum_capacity)
	, pressure_(pressure)
{
	;
}


double Tank::add(double quantity, double temperature)
{
	const double water_boil_temperature = Physics::compute_water_boil_temperature(pressure_);
	double specific_heat = Physics::WaterSpecificHeat;
	if(temperature > water_boil_temperature)
	{
		specific_heat = Physics::SteamSpecificHeat;
	}

	if(maximum_capacity_ - quantity_ <= quantity)
	{
		quantity = maximum_capacity_ - quantity_;
	}

	double thermal_energy_added = (quantity * temperature) * specific_heat;
	thermal_energy_ += thermal_energy_added;
	quantity_ += quantity;

	temperature_ = thermal_energy_ / (quantity_ * specific_heat);

	return quantity;
}

double Tank::take(double& quantity, double& temperature)
{
	double thermal_energy_quantity = 0;

	temperature = temperature_;
	if(quantity >= quantity_)
	{
		quantity = quantity_;
	}

	if(quantity_ < 0.01)
	{
		return 0;
	}
	else
	{
		double thermal_energy_xfer = quantity / quantity_;
		//if(temperature_ >= Physics::compute_water_boil_temperature(pressure_))
		//{
		//	thermal_energy_xfer *= 0.5;
		//}

		thermal_energy_quantity = thermal_energy_xfer * thermal_energy_;
		thermal_energy_ -= thermal_energy_quantity;
		quantity_ -= quantity;
	}

	return thermal_energy_quantity;
}

void Tank::transfer_thermal_energy(double energy)
{
	thermal_energy_ += energy;
	temperature_ = thermal_energy_ / (quantity_ * Physics::WaterSpecificHeat);
}