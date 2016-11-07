#include "pipe.h"

Pipe::Pipe(const std::string& name)
	: name_(name)
{

}
	
const std::string& Pipe::get_name()
{
	return name_;
}

void Pipe::update(float dt)
{

}
