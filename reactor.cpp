#include "reactor.h"
#include "physics.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sstream>

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
	ss<<"Simulation time [Day: " << days + 1 << " " << hours << ":" << minutes << ":" << seconds <<"]\n";
	date_string_ = ss.str();
	std::cerr<<date_string_;

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

	std::cerr<<"Reactor inner cooling loop temperature: "<<generator_.get_hot_tank()->get_temperature()<<"\n";
	std::cerr<<"Reactor core temperature: "<<core_.get_temperature()<<"\n";
	std::cerr<<"Reactor steam generating: "<<-energy_removal / 1e6 / dt<<" MW\n";


	// automatic control rod actuation
	if(core_.get_temperature() > 400.0)
	{
		float rods = core_.get_control_rod_position();
		if(rods < 1.0)
		{
			rods += 0.001;
			core_.set_control_rod_position(rods);
		}
	}
}