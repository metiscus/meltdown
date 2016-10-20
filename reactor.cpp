#include "reactor.h"
#include "physics.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

Reactor::Reactor()
	: coolant_qty_(100000.0)
	, coolant_temperature_(100.0)
	, time_(0)
{
	core_.set_control_rod_position(0.05);
	core_.add_coolant(coolant_qty_, coolant_temperature_);
//test
	coolant_pump_.set_is_powered(true);
	coolant_pump_.set_max_flow_rate(50000.0);
	coolant_pump_.set_flow_rate(37852.5);
	coolant_pump_.set_add_tank(core_.get_tank());
	coolant_pump_.set_take_tank(generator_.get_hot_tank());

	coolant_pump_2_.set_is_powered(true);
	coolant_pump_2_.set_is_flawless(true);
	coolant_pump_2_.set_max_flow_rate(50000.0);
	coolant_pump_2_.set_flow_rate(37852.5);
	coolant_pump_2_.set_add_tank(generator_.get_hot_tank());
	coolant_pump_2_.set_take_tank(core_.get_tank());

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
}

void Reactor::update(float dt)
{
	time_ += dt;

	int days = time_ / 3600 / 24;
	int hours = (int)(time_ / 3600) % 24;
	int minutes = (int)(time_ / 60) % 60;
	int seconds = ((int)time_) % 60;

	std::cerr<<"Simulation time [Day: " << days + 1 << " " << hours << ":" << minutes << ":" << seconds <<"]\n";

	coolant_pump_2_.update(dt);
	main_feedwater_pump_.update(dt);
	//high_pressure_injection_pump_.update(dt);
	core_.update(dt);
	generator_.update(dt);
	coolant_pump_.update(dt);

	// for now let's simulate the turbines removing heat from the cold side of the steam generator
	generator_.get_cold_tank()->transfer_thermal_energy(-generator_.get_cold_tank()->get_thermal_energy() * 0.01 * dt);
	
#if 0
	//coolant_flow_rate_ is in kg/minute
	double coolant_in = coolant_pump_.get_flow_rate() / 60.0 * dt;
	if(coolant_pump_.get_is_failed())
	{
		std::cerr<<"The main reactor coolant pump has failed!\n";
		coolant_in = 0.0f;
	}

	if(coolant_in < 0.001f)
	{
		if(high_pressure_injection_pump_.get_is_powered() && high_pressure_injection_pump_.get_is_failed())
		{
			std::cerr<<"The high pressure injection pump has failed. ECCS is OFFLINE!\n";
		}
		else
		{
			if(!high_pressure_injection_pump_.get_is_powered())
			{
				high_pressure_injection_pump_.set_is_powered(true);
				std::cerr<<"The high pressure injection pump has started. ECCS is ONLINE!\n";
			}
		}
	}
	
	if(high_pressure_injection_pump_.get_is_powered() && !high_pressure_injection_pump_.get_is_failed())
	{
		coolant_in += high_pressure_injection_pump_.get_flow_rate() / 60.0 * dt;
		std::cerr<<"\nWarning: ECCS is ONLINE!\n\n";
		usleep(10000);
	}



	if(coolant_in > coolant_qty_)
	{
		coolant_in = coolant_qty_;
	}

	// add coolant to the reactor
	core_.add_coolant(coolant_in, coolant_temperature_);
	coolant_qty_ -= coolant_in;

	// simulate the reactor
	core_.update(dt);

	// take coolant from the reactor
	float coolant_out_temp = core_.remove_coolant(coolant_in);

	double total_energy = Physics::WaterSpecificHeat * (1e3 * coolant_qty_ * coolant_temperature_ + 1e3 * coolant_in * coolant_out_temp);
	coolant_qty_ += coolant_in;
	coolant_temperature_ = total_energy / coolant_qty_ * 1e-3 / Physics::WaterSpecificHeat;
	
	double generator_flow = 10.0 * coolant_in;
	//begin test
	generator_.add_cold(generator_flow, 20.0);
	//end test
	generator_.add_hot(generator_flow, coolant_temperature_);
	generator_.update(dt);
	coolant_out_temp = generator_.remove_hot(generator_flow);

	double energy_to_remove = (coolant_temperature_ - coolant_out_temp) * generator_flow * 1e3 * Physics::WaterSpecificHeat;
	total_energy -= energy_to_remove;
	coolant_temperature_ = total_energy / coolant_qty_ * 1e-3 / Physics::WaterSpecificHeat;

	//begin test
	generator_.remove_cold(generator_flow);
	//end test
#endif


	std::cerr<<"Reactor inner cooling loop temperature: "<<generator_.get_hot_tank()->get_temperature()<<"\n";
	std::cerr<<"Reactor core temperature: "<<core_.get_temperature()<<"\n";
	//std::cerr<<"Reactor steam generator return: "<<coolant_out_temp<<"\n";
}