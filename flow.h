#pragma once

class Flow
{
	double temperature_; // kelvin
	double m_dot_; //kg/s
	double pressure_; // N/m^2
	double density_; // kg / m^3

public:
	Flow()
	{
		temperature_ = 300;
		m_dot_ = 0;
		pressure_ = 0.0;
		density_ = 1000.0; 
	}

	inline void   set_temperature(double temperature) { temperature_ = temperature; }
	inline void   set_mass_flow_rate(double m_dot) { m_dot_ = m_dot; }
	inline void   set_pressure(double pressure) { pressure_ = pressure; }
	inline void   set_density(double density) { density_ = density; }

	inline double get_temperature() const { return temperature_; }
	inline double get_mass_flow_rate() const { return m_dot_; }
	inline double get_volume_flow_rate() const { return m_dot_ / density_; }
	inline double get_pressure() const { return pressure_; }
	inline double get_density() const { return density_; }
};