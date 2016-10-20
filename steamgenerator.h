#pragma once

class SteamGenerator
{
	float transfer_coefficient_;
	float hot_quantity_;
	float hot_temperature_;
	float cold_quantity_;
	float cold_temperature_;


public:
	SteamGenerator();

	void add_hot(float quantity, float temperature);
	void add_cold(float quantity, float temperature);

	void update(float dt);

	float remove_hot(float quantity);
	float remove_cold(float quantity);
};