#pragma once

#include "tank.h"

class SteamGenerator
{
	float transfer_coefficient_;

	Tank cold_tank_;
	Tank hot_tank_;

public:
	SteamGenerator();

	void add_hot(double& quantity, double temperature);
	void add_cold(double& quantity, double temperature);

	void update(float dt);

	double remove_hot(double quantity);
	double remove_cold(double quantity);

	Tank* get_cold_tank() { return &cold_tank_; }
	Tank* get_hot_tank() { return &hot_tank_; }
};