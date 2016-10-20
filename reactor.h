#pragma once

#include "core.h"
#include "steamgenerator.h"
#include "pump.h"

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

public:
	Reactor();
	inline const Core& get_core() const { return core_; }
	inline Core& get_core() { return core_; }

	void update(float dt);
};