#pragma once

#include <cmath>

namespace Physics
{
	// this is in units of j/(g*K)
	static constexpr double WaterSpecificHeat   = 4.184;
	static constexpr double SteamSpecificHeat   = 2.080;
	static constexpr double UraniumSpecificHeat = 0.116;

	static constexpr double WaterVolumeExpansion = 1.72e-3;

	static constexpr double PSIToKPa = 6.895;

	inline double psi_to_kpa(double psi)
	{
		return psi * 6.895;
	}

	inline double kpa_to_psi(double kpa)
	{
		return kpa * 1.0 / (6.895);
	}

	inline double compute_water_boil_temperature(double pressure)
	{
		//const double hvap = 40.66; //kj/mol
		//const double T0 = 100.0;
		//const double P0 = 101.325e3; // 1 atm
		//const double R = 8.3144589e-5;
		//double boiling_point = 1.0 / ((1.0 / T0) - (R*log((pressure * PSIToPa)/P0)) / hvap);
		double boiling_point = log((pressure * PSIToKPa) / 66.5238) / 0.015758;
		return boiling_point;
	}

	namespace Units
	{
		enum Temperature
		{
			Kelvin,
			Fahrenheit,
			Celsius
		};

		double Convert(Temperature from, Temperature to, double value);
	}
}