#include "core.h"
#include "physics.h"
#include <cassert>
#include <iostream>
#include "log.h"

Core::Core()
	: control_rods_(1.0f)
	, max_power_(2568e6f)
	//, temperature_coefficient_(0.002)
	, temperature_coefficient_(0.0000)
	, boiling_(false)
	, fuel_cladding_melt_temp_(726.0)
	, fuel_cladding_integrity_(1.0)
	, reactor_vessel_(100000.0, 2600*Physics::PSIToKPa)
	, is_scrammed_(false)
	, uranium_mass_(1000.0)
	, fuel_linear_heat_rate_(44.5246) // kW/m
	, fuel_rod_count_(50968)
	, fuel_rod_radius_(0.009) // m
	, fuel_rod_length_(3.658)  // m
	, fuel_temperature_(300.0)
{
	double quantity = 100000.0;
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

void Core::update_core(float dt)
{
	//TEST: the estimated flow rate is 10000 gal/minute
	// I think this is roughly 63.08 kg/sec
	//inlet_.set_mass_flow_rate(63.08);
	//inlet_.set_mass_flow_rate(1000.08);
	inlet_.set_mass_flow_rate(1500.08);

	// Compute the maximum thermal output of the reactor
	constexpr double PeakToAverageRatio = 2.5;
	constexpr double FuelPowerRatio     = 0.974;

	// this has units of W
	const double maximum_thermal_output = FuelPowerRatio * 1e3 * (fuel_rod_length_ * fuel_linear_heat_rate_ * fuel_rod_count_) / (PeakToAverageRatio * FuelPowerRatio);

	// the moderator_temperature_coefficient reflects the change in effectiveness of the neutron moderator
	// as the moderator temperature changes
	const double moderator_temperature_coefficient = 0.00001; // percent / Kelvin
	double coolant_temperature_factor = 1.0 - inlet_.get_temperature() * moderator_temperature_coefficient;

	// the void_coefficient determines how much the boiling of water reduces the reactivity
	//TODO
	const double void_coefficient = 0.0;
	double void_factor = 1.0 - void_coefficient;

	// How effective the control rods are at absorbing neutron flux
	const double control_rod_factor = 1.0 - 0.95 * control_rods_;

	// the decay heat is the amount of total output even when powered down
	const double decay_heat = 0.01;

	double reactivity = coolant_temperature_factor * control_rod_factor * void_factor;
	if(reactivity < decay_heat)
	{
		reactivity = decay_heat;
	}

	// this has units of W
	double thermal_output = reactivity * maximum_thermal_output;

	std::cerr<<"The reactor is producing "<< thermal_output * 1e-6 <<" MW of thermal output\n";

	// compute the mass of the fuel in the reactor
	constexpr double rho_u235 = 19100.0e3; // g / m^3

	const double rod_volume = 3.14159 * fuel_rod_radius_ * fuel_rod_radius_ * fuel_rod_length_;
	
	// units of grams
	double uranium_mass = rho_u235 * rod_volume * fuel_rod_count_;
	//std::cerr<<"Uranium mass: "<< uranium_mass * 1e-6 << " Metric Tons\n";

	// compute the temperature of the uranium if all of the heat was stored in it
	fuel_temperature_ += (thermal_output * dt) / (Physics::UraniumSpecificHeat * uranium_mass);
	std::cerr<<"The uranium temperature is: "<<fuel_temperature_ - 273.15<<"\n";

	// Compute the area of the reactor vessel
	//TODO: improve this, there wouldn't be room for the water
	double rods_area = pow(2.0 * fuel_rod_radius_, 2.0) * fuel_rod_count_;
	//double reactor_diameter = sqrtf(rods_area);
	double reactor_diameter = sqrtf(rods_area) * 1.25;
	double reactor_area = 3.14159 * (0.25 * reactor_diameter * reactor_diameter);

	// Compute the reynolds number of the flow
	constexpr double dynamic_viscosity_of_water = 0.0198e6; // Pa * s
	const double kinematic_viscoscity = dynamic_viscosity_of_water / inlet_.get_density();

	double reynolds_number = (inlet_.get_volume_flow_rate() * reactor_diameter) / (kinematic_viscoscity * reactor_area);
	//std::cerr<<"The Reynolds number for the flow in the reactor is: "<<reynolds_number<<"\n";

	// compute the heat transfer coefficient for the reactor
	constexpr double WaterConductivity = 0.6864; // W/(m*K)
	constexpr double WaterPrandtlNumber = (Physics::WaterSpecificHeat * dynamic_viscosity_of_water) / WaterConductivity;
	constexpr double beta = 1200.0;
	
	double temperature_difference = fuel_temperature_ - inlet_.get_temperature();
	if(temperature_difference > 0.0)
	{
		std::cerr<<"temperature_difference: "<< temperature_difference<<"\n";
		//we will just not transfer heat from the core into the water
		double grashof_number = 9.81 * beta * (temperature_difference) * pow(reactor_diameter, 3.0) / kinematic_viscoscity;

		double raleigh_number = grashof_number * WaterPrandtlNumber;
		double heat_xfer_coefficient = WaterConductivity * 0.54 * pow(raleigh_number, 0.25) / fuel_rod_length_;

		//std::cerr<<"Heat transfer coefficient: "<<heat_xfer_coefficient<<"\n";

		// compute the heat flux into the water
		//double xfer_area = (fuel_rod_radius_ * 2 * 3.14159) * fuel_rod_length_ * fuel_rod_count_;
		double xfer_area = 7.00 * (fuel_rod_radius_ * 2 * 3.14159) * fuel_rod_length_ * fuel_rod_count_;
		double heat_flux = heat_xfer_coefficient * (temperature_difference) * xfer_area;

		// compute outlet temperature
		double coolant_mass = 1e3 * inlet_.get_mass_flow_rate() * dt;
		double heat_transferred = heat_flux * dt;
		double temperature_change = (1.0 / Physics::WaterSpecificHeat) * heat_transferred / (coolant_mass);
		
		reactor_vessel_.transfer_thermal_energy( heat_transferred );

		// guesstimate from http://www.thermopedia.com/content/1150/
		//double water_volume_temperature_factor = (350.0 - (inlet_.get_temperature() - 273.15)) * 0.0069;
		//outlet_.set_density(inlet_.get_density() + (Physics::WaterVolumeExpansion + water_volume_temperature_factor) * temperature_change);

		//std::cerr<<"The Heat flux is: "<< heat_flux <<"\n";
		// remove the energy from the rods and add it into the outbound stream
		fuel_temperature_ -= (heat_transferred) / (Physics::UraniumSpecificHeat * uranium_mass);	

		std::cerr<<"The uranium temperature is NOW: "<<  fuel_temperature_ - 273.15 <<"\n";


		std::cerr<<"Temperature change of coolant is: "<< temperature_change <<"\n";
		outlet_.set_temperature(inlet_.get_temperature() + temperature_change);
	}
	else
	{
		outlet_.set_temperature(inlet_.get_temperature());
		outlet_.set_density(inlet_.get_density());
		std::cerr<<"*** Temperature mismatch *** \n";
	}

	// conservation of mass
	outlet_.set_mass_flow_rate(inlet_.get_mass_flow_rate());
	outlet_.set_pressure(inlet_.get_pressure());
	
}

void Core::update(float dt)
{
	if(is_scrammed_)
	{
		if(control_rods_ < 1.0f)
		{
			control_rods_ += 0.01 * dt;
		}
	}

	std::cerr<<"Inlet temperature: "<<inlet_.get_temperature() - 273.15<<"\n";

	update_core(dt);

	std::cerr<<"Outlet temperature: "<<outlet_.get_temperature() - 273.15<<"\n";

	// check for fuel cladding melt
	if((fuel_temperature_ - 273.15) > fuel_cladding_melt_temp_)
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
	return fuel_temperature_;
}

void Core::set_uranium_mass(double mass)
{
	assert(mass >= 0.0);
	uranium_mass_ = mass;
}