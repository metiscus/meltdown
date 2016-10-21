#pragma once

#include "core.h"
#include "steamgenerator.h"
#include "pump.h"
#include <vector>
#include <map>
#include <string>

class Reactor
{
	Core core_;
	double coolant_qty_;
	double coolant_temperature_;
	double time_;
	SteamGenerator generator_;

	Pump coolant_pump_;
	Pump coolant_pump_2_;
	Pump main_feedwater_pump_;
	Pump main_feedwater_pump_2_;
	Pump emergency_feedwater_pump_;
	Pump sump_pump_;
	Pump high_pressure_injection_pump_;

	std::vector<Pump*> pumps_;

	std::string date_string_;

public:
	Reactor();
	inline const Core& get_core() const { return core_; }
	inline Core& get_core() { return core_; }

	void update(float dt);

	inline std::vector<Pump*> get_pumps() { return pumps_; }
	std::vector<const Pump*> get_pumps() const { return std::vector<const Pump*>(pumps_.begin(), pumps_.end()); }
	
	std::map<std::string, const Pump*> get_pump_map() const
	{
		std::map<std::string, const Pump*> map;
		for(int ii=0; ii<pumps_.size(); ++ii)
		{
			map.insert(std::make_pair(pumps_[ii]->get_name(), pumps_[ii]));
		}
		return map;
	}

	inline float get_core_fluid_fill_percent() const
	{
		auto tank = core_.get_tank();
		return tank->get_quantity() / tank->get_maximum_capacity();
	}

	inline std::string get_date_string() const { return date_string_; }
};