#include "physics.h"

namespace Physics
{

	namespace Units
	{
		double Convert(Temperature from, Temperature to, double value)
		{
			double kelvin = 0.0;
			double ret = 0.0;
			switch(from)
			{
				case Kelvin: kelvin = value; break;
				case Celsius: kelvin = value - 273.15; break;
				case Fahrenheit: kelvin = (5.0/9.0)*(value - 32.0) + 273.15; break;
			}

			switch(to)
			{
				case Kelvin: ret = kelvin; break;
				case Celsius: ret = kelvin - 273.15; break;
				case Fahrenheit: ret = (9.0/5.0)*(value - 459.67); break;
			}

			return ret;
		}
	}
}