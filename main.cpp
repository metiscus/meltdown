#include "reactor.h"
#include <iostream>

int main(int argc, char** argv)
{
	Reactor reactor;

	for(int ii=0; ii<60*60*100; ++ii)
	{
		reactor.update(1.0/60.0);
	}

	return 0;
}