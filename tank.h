#pragma once

class Tank
{
	double quantity_; 			// g
	double temperature_;		// kelvin
	double thermal_energy_;		// joules
	double maximum_capacity_;	// g
	double pressure_;			// Pa

public:
	Tank(double maximum_capacity, double pressure = 101.325);

	inline double get_quantity() const { return quantity_; }
	inline double get_temperature() const { return temperature_; }
	inline double get_thermal_energy() const { return thermal_energy_; }
	inline double get_maximum_capacity() const { return maximum_capacity_; }
	inline double get_pressure() const { return pressure_; }

	double add(double quantity, double temperature);
	// return the amount of thermal energy taken
	double take(double& quantity, double& temperature);

	void transfer_thermal_energy(double energy);
};