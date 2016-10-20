#include "steamgenerator.h"
#include "physics.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>

SteamGenerator::SteamGenerator()
	: transfer_coefficient_(0.95)
	, hot_quantity_(6000)
	, hot_temperature_(0)
	, cold_quantity_(6000)
	, cold_temperature_(1.0)
{
	;
}

void SteamGenerator::add_hot(float quantity, float temperature)
{
	//std::cout<<"[SteamGenerator] hot in_temp: " << temperature << "\n";
	//std::cout<<"[SteamGenerator] hot in_qty: " << quantity << "\n";
	///std::cout<<"[SteamGenerator] hot before: " << hot_temperature_ << "\n";
	double hot_energy_original = hot_quantity_ * 1e3 * hot_temperature_ * Constants::WaterSpecificHeat;
	double hot_energy_in = temperature * quantity * 1e3 * Constants::WaterSpecificHeat;
	double hot_energy = hot_energy_in + hot_energy_original;
	hot_quantity_ += quantity;
	hot_temperature_ = hot_energy / (hot_quantity_ * 1e3 * Constants::WaterSpecificHeat);
	//std::cout<<"[SteamGenerator] hot after: " << hot_temperature_ << "\n";
}

void SteamGenerator::add_cold(float quantity, float temperature)
{
	double energy_original = cold_quantity_ * 1e3 * cold_temperature_ * Constants::WaterSpecificHeat;
	double energy_in = temperature * quantity * 1e3 * Constants::WaterSpecificHeat;
	double energy = energy_in + energy_original;
	cold_quantity_ += quantity;
	cold_temperature_ = energy / (cold_quantity_ * 1e3 * Constants::WaterSpecificHeat);
}

void SteamGenerator::update(float dt)
{
	//todo: improve this
	double hot_energy  = hot_quantity_ * 1e3 * hot_temperature_ * Constants::WaterSpecificHeat;
	double cold_energy = cold_quantity_ * 1e3 * cold_temperature_ * Constants::WaterSpecificHeat;
	double energy_transfered = transfer_coefficient_ * std::max(hot_temperature_ - cold_temperature_, 0.0f) * fabs(hot_energy - cold_energy);
	//std::cout<<"[SteamGenerator] hot energy: " << hot_energy << "\n";
	//std::cout<<"[SteamGenerator] cold energy: " << cold_energy << "\n";
	//std::cout<<"[SteamGenerator] energy transferred: " << energy_transfered << "\n";

	cold_energy += energy_transfered;
	hot_energy -= energy_transfered;

	hot_temperature_  = hot_energy / hot_quantity_ * 1e-3 / Constants::WaterSpecificHeat;
	cold_temperature_ = cold_energy / cold_quantity_ * 1e-3 / Constants::WaterSpecificHeat;

	//std::cout<<"[SteamGenerator] hot: " << hot_temperature_ << "\n";
	//std::cout<<"[SteamGenerator] cold: " << cold_temperature_ << "\n";
}

float SteamGenerator::remove_hot(float quantity)
{
	hot_quantity_ -= quantity;
	return hot_temperature_;
}

float SteamGenerator::remove_cold(float quantity)
{
	cold_quantity_ -= quantity;
	return cold_temperature_;
}