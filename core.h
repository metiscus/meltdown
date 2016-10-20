#pragma once

#include "tank.h"

class Core
{
	float control_rods_;
	float max_power_;
	double temperature_coefficient_;
	bool boiling_;
	const float fuel_cladding_melt_temp_;

	float fuel_cladding_integrity_;

	Tank reactor_vessel_;

public:
	Core();

	void set_control_rod_position(float pos);
	void set_max_power(float heat);

	void update(float dt);

	void add_coolant(double quantity, double temperature);
	double remove_coolant(double& quantity);

	const float get_temperature() const;

	Tank* get_tank() { return &reactor_vessel_; }
};