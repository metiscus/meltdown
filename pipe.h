#pragma once
#include <string>

class Pipe
{
	std::string name_;
	double mass_flow_rate_;
	double heat_transfer_rate_;
	double head_loss_;

	std::vector<Valve> inputs_;

public:
	Pipe(const std::string& name);
	const std::string& get_name();
	void update(float dt);

	void   set_mass_flow_rate(double m_dot);
	double get_mass_flow_rate() const;

	void set_heat_transfer_rate(double q_dot);
	double get_heat_transfer_rate() const;

	void set_head_loss(double head_loss);
	double get_head_loss() const;
};