#pragma once

class Core
{
	float control_rods_;
	float max_power_;
	double thermal_energy_;
	float pressure_;
	float max_pressure_;
	float coolant_quantity_;
	double temperature_coefficient_;
	bool boiling_;
	const float fuel_cladding_melt_temp_;

	float fuel_cladding_integrity_;

public:
	Core();

	void set_control_rod_position(float pos);
	void set_max_power(float heat);

	void update(float dt);

	void add_coolant(float quantity, float temperature);
	float remove_coolant(float quantity);

	const float get_temperature() const;
};