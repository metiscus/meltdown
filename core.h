#pragma once

#include "tank.h"
#include "flow.h"

class Core
{
	float control_rods_;
	float max_power_;
	double temperature_coefficient_;
	bool boiling_;
	const float fuel_cladding_melt_temp_;

	float fuel_cladding_integrity_;

	Tank reactor_vessel_;

	bool is_scrammed_;

	double uranium_mass_;

	const double fuel_linear_heat_rate_;
	double fuel_rod_count_;
	double fuel_rod_radius_;	// meters
	double fuel_rod_length_;	// meters
	double fuel_temperature_;	// kelvin

	double power_hours_;

	Flow inlet_;
	Flow outlet_;

public:
	Core();

	void set_control_rod_position(float pos);
	void set_max_power(float heat);

	void update(float dt);

	void add_coolant(double quantity, double temperature);
	double remove_coolant(double& quantity);

	const float get_temperature() const;

	Tank* get_tank() { return &reactor_vessel_; }
	const Tank* get_tank() const { return &reactor_vessel_; }
	inline float get_control_rod_position() const { return control_rods_; }

	void set_scram(bool scram) { is_scrammed_ = scram; }

	void set_uranium_mass(double mass);

	inline const Flow& get_inlet() const { return inlet_; }
	inline Flow& get_inlet() { return inlet_; }

	inline const Flow& get_outlet() const { return outlet_; }
	inline Flow& get_outlet() { return outlet_; }

private:
	void update_core(float dt);
};