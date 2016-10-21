#include "steamgenerator.h"
#include "physics.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cassert>

SteamGenerator::SteamGenerator()
	: transfer_coefficient_(0.95)
	, cold_tank_(60000.0, 2600*Physics::PSIToKPa)
	, hot_tank_(60000.0, 2600*Physics::PSIToKPa)
{
	double quantity = 50000.0;
	hot_tank_.add(quantity, 100);

	quantity = 50000.0;
	cold_tank_.add(quantity, 20);
}

void SteamGenerator::add_hot(double &quantity, double temperature)
{
	quantity = hot_tank_.add(quantity, temperature);
}

void SteamGenerator::add_cold(double &quantity, double temperature)
{
	quantity = cold_tank_.add(quantity, temperature);
}

void SteamGenerator::update(float dt)
{
	//todo: improve this
	double hot_energy  = hot_tank_.get_thermal_energy();
	double cold_energy = cold_tank_.get_thermal_energy();
	double energy_transfered = transfer_coefficient_ * std::min(1.0, std::max(hot_tank_.get_temperature() - cold_tank_.get_temperature(), 0.0)) * fabs(hot_energy - cold_energy);
	if(energy_transfered > hot_energy)
	{
		energy_transfered = hot_energy;
	}

	cold_tank_.transfer_thermal_energy(energy_transfered);
	hot_tank_.transfer_thermal_energy(-energy_transfered);

	//assert(energy_transfered <= hot_energy);
	//std::cout<<"[SteamGenerator] hot energy: " << hot_energy << "\n";
	//std::cout<<"[SteamGenerator] cold energy: " << cold_energy << "\n";
	//std::cout<<"[SteamGenerator] energy transferred: " << energy_transfered << "\n";



	//std::cout<<"[SteamGenerator] hot: " << hot_tank_.get_temperature() << "\n";
	//std::cout<<"[SteamGenerator] cold: " << cold_tank_.get_temperature() << "\n";
}

double SteamGenerator::remove_hot(double quantity)
{
	double temp;
	return hot_tank_.take(quantity, temp);
}

double SteamGenerator::remove_cold(double quantity)
{
	double temp;
	return cold_tank_.take(quantity, temp);
}