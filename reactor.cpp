#include "reactor.h"
#include "physics.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <iomanip>

Reactor::Reactor()
	: coolant_qty_(60000.0)
	, coolant_temperature_(100.0)
	, time_(0)
	, coolant_pump_("Reactor Coolant")
	, coolant_pump_2_("Reactor Coolant Return")
	, thermal_coolant_pump_("Reactor Coolant Thermal Flow")
	, thermal_coolant_pump_2_("Reactor Coolant Return Thermal Flow")
	, main_feedwater_pump_("Main Feedwater")
	, main_feedwater_pump_2_("Main Feedwater Return")
	, emergency_feedwater_pump_("Emergency Feedwater Return")
	, sump_pump_("Containment Building Sump")
	, high_pressure_injection_pump_("High Pressure Injection")
	, flow_tank_(1000e6)
{
	//core_.set_control_rod_position(0.05);
	core_.add_coolant(coolant_qty_, coolant_temperature_);
//test
	coolant_pump_.set_is_powered(true);
	coolant_pump_.set_max_flow_rate(50000.0);
	coolant_pump_.set_is_flawless(true);
	coolant_pump_.set_flow_rate(37852.5);
	coolant_pump_.set_add_tank(core_.get_tank());
	coolant_pump_.set_take_tank(generator_.get_hot_tank());

	coolant_pump_2_.set_is_powered(true);
	coolant_pump_2_.set_is_flawless(true);
	coolant_pump_2_.set_max_flow_rate(50000.0);
	coolant_pump_2_.set_flow_rate(37852.5);
	coolant_pump_2_.set_add_tank(generator_.get_hot_tank());
	coolant_pump_2_.set_take_tank(core_.get_tank());

	thermal_coolant_pump_.set_is_powered(false);
	thermal_coolant_pump_.set_max_flow_rate(1000.0);
	thermal_coolant_pump_.set_is_flawless(true);
	thermal_coolant_pump_.set_add_tank(core_.get_tank());
	thermal_coolant_pump_.set_flow_rate(0.0);
	thermal_coolant_pump_.set_take_tank(generator_.get_hot_tank());

	thermal_coolant_pump_2_.set_is_powered(false);
	thermal_coolant_pump_2_.set_max_flow_rate(1000.0);
	thermal_coolant_pump_2_.set_is_flawless(true);
	thermal_coolant_pump_2_.set_add_tank(generator_.get_hot_tank());
	thermal_coolant_pump_2_.set_flow_rate(0.0);
	thermal_coolant_pump_2_.set_take_tank(core_.get_tank());

	high_pressure_injection_pump_.set_is_powered(false);
	high_pressure_injection_pump_.set_max_flow_rate(50000.0);
	high_pressure_injection_pump_.set_flow_rate(20000.0);

	main_feedwater_pump_.set_is_powered(true);
	main_feedwater_pump_.set_max_flow_rate(50000.0);
	//main_feedwater_pump_.set_flow_rate(37852.0);
	main_feedwater_pump_.set_flow_rate(10000.0);
	//main_feedwater_pump_.set_take_tank(generator_.get_hot_tank());
	main_feedwater_pump_.set_add_tank(core_.get_tank());

	main_feedwater_pump_2_.set_is_flawless(true);
	main_feedwater_pump_2_.set_is_powered(true);
	main_feedwater_pump_2_.set_max_flow_rate(50000.0);
	main_feedwater_pump_2_.set_flow_rate(10000.0);
	//main_feedwater_pump_2_.set_take_tank(generator_.get_cold_tank());
	//main_feedwater_pump_2_.set_add_tank(core_.get_tank());

	flow_tank_.add(600000, 350);

	pumps_ = {&coolant_pump_, &coolant_pump_2_, &main_feedwater_pump_, &main_feedwater_pump_2_, &emergency_feedwater_pump_, &sump_pump_, &high_pressure_injection_pump_};
}

void Reactor::update(float dt)
{
	time_ += dt;

	int days = time_ / 3600 / 24;
	int hours = (int)(time_ / 3600) % 24;
	int minutes = (int)(time_ / 60) % 60;
	int seconds = ((int)time_) % 60;

	std::stringstream ss;
	ss<<"Simulation time [Day: " << days + 1 << " " << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds <<"]\n";
	date_string_ = ss.str();
	std::cerr<<date_string_;

#if 0
	coolant_pump_2_.update(dt);
	if(coolant_pump_2_.get_is_failed())
	{
		core_.set_scram(true);
		std::cerr<<"***** The Reactor is SCRAMed *****\n";
		coolant_pump_.set_is_powered(false);
		thermal_coolant_pump_.set_is_powered(true);
		thermal_coolant_pump_.set_flow_rate(0.05 * core_.get_temperature());
		thermal_coolant_pump_2_.set_is_powered(true);
		thermal_coolant_pump_2_.set_flow_rate(0.05 * core_.get_temperature());
	}

	thermal_coolant_pump_2_.update(dt);
	//main_feedwater_pump_.update(dt);
	coolant_pump_.update(dt);
	
	//high_pressure_injection_pump_.update(dt);
	core_.update(dt);
	generator_.update(dt);
	thermal_coolant_pump_.update(dt);	

	//TODO: there is convective flow even when the pumps are broken
	// because the reactor core generates so much heat, it is capable
	// of keeping the fluid flowing around the cooling loop even when the pumps
	// fail. This must be modeled


	// for now let's simulate the turbines removing heat from the cold side of the steam generator
	double energy_removal = -generator_.get_cold_tank()->get_thermal_energy() * 0.0055 * dt;
	generator_.get_cold_tank()->transfer_thermal_energy(energy_removal);

#endif
	const Flow& core_out = core_.get_outlet();
	Flow& core_in = core_.get_inlet();

	//core_in.set_temperature(273.15 + 300.0);

	// just for testing pump the core then remove some heat from the fluid
	// then tie the output of the core back to the inlet of the core
	core_.update(dt);
	
	const double flow_in_temperature = core_out.get_temperature();
	const double flow_volume = core_out.get_mass_flow_rate() * dt * 1e3;

	std::cerr<<"Flow rate: "<<flow_volume<<"\n";

	flow_tank_.add(flow_volume, core_out.get_temperature());

	// model energy removal
	static double energy_removal_average = 0.0;
	double energy_removal = 0.0;
	if(flow_tank_.get_temperature() - 273.15 > 300)
	{
		double removal_ratio = 0.70 * (double)flow_volume / (double)flow_tank_.get_quantity();

		energy_removal = - removal_ratio * flow_tank_.get_thermal_energy() * dt;
		flow_tank_.transfer_thermal_energy(energy_removal);
	}

	// model looping back coolant
	double qty = flow_volume;
	double temperature = 10;

	flow_tank_.take(qty, temperature);

	//energy_removal_average = 0.01 * energy_removal + 0.99 * energy_removal_average;
	double energy_removed = (temperature - flow_in_temperature) * flow_volume * Physics::WaterSpecificHeat;
	energy_removal_average = 0.01 * energy_removed + 0.99 * energy_removal_average;

	std::cerr<<"Reactor inner cooling loop temperature: "<<flow_tank_.get_temperature() - 273.15<<"\n";
	std::cerr<<"Reactor core temperature: "<<core_.get_temperature() - 273.15<<"\n";
	std::cerr<<"Reactor steam generating: "<<-energy_removal_average / 1e6 / dt <<" MW\n";

	// automatic control rod actuation
	constexpr double Ki = 0.001;
	constexpr double Kib = 0.00001;
	constexpr double Kd = 0.2;
	constexpr double Kp = 1.0;
	static double i_error = 0.0;
	double error = 350.0 - (core_.get_temperature() - 273.15);
	static double error_prev = error;

	// Integrator
	i_error += dt * error;
	i_error *= (1.0 - Kib);

	double movement = Kp * error + Kd * (error - error_prev) / dt + Ki;
	error_prev = error;

	float rods = core_.get_control_rod_position();
	if(rods < 1.0)
	{
		rods -= movement * dt;
		core_.set_control_rod_position(rods);
	}

#if 0
	if(core_.get_temperature() - 273.15 > 400.0)
	{
		float rods = core_.get_control_rod_position();
		if(rods < 1.0)
		{
			rods += 0.001;
			core_.set_control_rod_position(rods);
		}
	}
#endif

	core_in.set_temperature(flow_tank_.get_temperature());
	core_in.set_density(core_out.get_density());
}