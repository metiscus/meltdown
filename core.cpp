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
	, fuel_temperature_(300)
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
	// this works out to about 0.6309 M3/sec
	inlet_.set_mass_flow_rate(inlet_.get_density() * 0.6309);

	// Compute the maximum thermal output of the reactor
	constexpr double PeakToAverageRatio = 2.5;
	constexpr double FuelPowerRatio     = 0.974;

	// this has units of kW
	const double maximum_thermal_output = 1e-3 * (fuel_rod_length_ * fuel_linear_heat_rate_ * fuel_rod_count_) / (PeakToAverageRatio * FuelPowerRatio);

	// the moderator_temperature_coefficient reflects the change in effectiveness of the neutron moderator
	// as the moderator temperature changes
	const double moderator_temperature_coefficient = 0.0001; // percent / Kelvin
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

	// this has units of kW
	double thermal_output = reactivity * maximum_thermal_output;

	std::cerr<<"The reactor is producing "<<thermal_output<<" kW of thermal output\n";

	// compute the mass of the fuel in the reactor
	constexpr double rho_u235 = 19050.0; // kg / m^3

	const double rod_volume = 3.14159*fuel_rod_radius_*fuel_rod_radius_*fuel_rod_length_;
	double uranium_mass = rho_u235 * rod_volume * fuel_rod_count_;

	// compute the temperature of the uranium if all of the heat was stored in it
	fuel_temperature_ += (thermal_output * dt)/ ( 1e-3 * Physics::UraniumSpecificHeat * uranium_mass);
	std::cerr<<"The uranium temperature is "<<fuel_temperature_<<"\n";

	// Compute the area of the reactor vessel
	//TODO: improve this, there wouldn't be room for the water
	double rods_area = pow(2.0 * fuel_rod_radius_, 2.0) * fuel_rod_count_;
	double reactor_diameter = sqrtf(rods_area);
	double reactor_area = 3.14159 * (0.25 * reactor_diameter * reactor_diameter);

	// Compute the reynolds number of the flow
	constexpr double dynamic_viscosity_of_water = 0.0198e6; // Pa * s
	const double kinematic_viscoscity = dynamic_viscosity_of_water / inlet_.get_density();

	double reynolds_number = (inlet_.get_volume_flow_rate() * reactor_diameter) / (kinematic_viscoscity * reactor_area);
	std::cerr<<"The Reynolds number for the flow in the reactor is: "<<reynolds_number<<"\n";

	// compute the heat transfer coefficient for the reactor
	constexpr double WaterConductivity = 0.6864; // W/(m*K)
	const double WaterPrandtlNumber = (1e-3 * Physics::WaterSpecificHeat * dynamic_viscosity_of_water) / WaterConductivity;
	constexpr double beta = 1200.0;
	double grashof_number = 9.81 * beta * (fuel_temperature_ - inlet_.get_temperature()) * pow(reactor_diameter, 3.0) / kinematic_viscoscity;
	double raleigh_number = grashof_number * WaterPrandtlNumber;
	double heat_xfer_coefficient = WaterConductivity * 0.54 * pow(raleigh_number, 0.25) / fuel_rod_length_;

	// compute the heat flux into the water
	double xfer_area = (fuel_rod_radius_ * 2 * 3.14159) * fuel_rod_length_ * fuel_rod_count_;
	double heat_flux = heat_xfer_coefficient * (fuel_temperature_ - inlet_.get_temperature());

	std::cerr<<"The Heat flux is: "<< heat_flux <<"\n";

	// remove the energy from the rods and add it into the outbound stream
	fuel_temperature_ -= (heat_flux * dt)/ ( 1e-3 * Physics::UraniumSpecificHeat * uranium_mass);
	std::cerr<<"The uranium temperature is NOW: "<<fuel_temperature_<<"\n";

	// conservation of mass
	outlet_.set_mass_flow_rate(inlet_.get_mass_flow_rate());
	outlet_.set_pressure(inlet_.get_pressure());
	outlet_.set_density(inlet_.get_density());

	// compute outlet temperature
	double coolant_mass = inlet_.get_mass_flow_rate() * dt;
	double temperature_change = (1.0 / Physics::WaterSpecificHeat) * heat_flux / (coolant_mass);
	std::cerr<<"Temperature change of coolant is: "<<temperature_change<<"\n";
	outlet_.set_temperature(inlet_.get_temperature() + temperature_change);

	reactor_vessel_.transfer_thermal_energy( heat_flux * 1e3 );
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

	update_core(dt);

	double thermal_coefficient = reactor_vessel_.get_temperature() * temperature_coefficient_;
	// boiling water moderates the reaction very poorly, I model this by increasing the temperature coefficient
	// however, boiling water can absorb less heat (~50% by mass) so that needs modeled
	// TODO: boiling water should increase the system pressure (steam explosion)
	if(boiling_)
	{
		thermal_coefficient *= 1.1;
	}

	double energy_generated_new    = dt * max_power_ * (1.0 - 0.95f * control_rods_) * (1.0 - thermal_coefficient);
	static double energy_generated = energy_generated_new;
	energy_generated = 0.9999 * energy_generated + 0.0001 * energy_generated_new;
	std::cerr<<"Core generated "<<energy_generated<< "energy\n";
	std::cerr<<"Pre xfer temp: " << reactor_vessel_.get_temperature() <<"\n";
	//reactor_vessel_.transfer_thermal_energy( energy_generated );
	std::cerr<<"Post xfer temp: " << reactor_vessel_.get_temperature() <<"\n";

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

void Core::set_uranium_mass(double mass)
{
	assert(mass >= 0.0);
	uranium_mass_ = mass;
}