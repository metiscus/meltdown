#include "loop.h"
#include "pipe.h"

Loop::Loop(const std::string& name)
	: name_(name)
{

}

void  Loop::add_pipe(Pipe pipe)
{
	pipes_.push_back(pipe);
}

Pipe* Loop::get_pipe(const std::string& id)
{
	Pipe *pipe = NULL;
	for(auto itr = pipes_.begin(); itr!=pipes_.end(); ++itr)
	{
		if(itr->get_name() == id)
		{
			pipe = &(*itr);
			break;
		}
	}

	return pipe;
}

void  Loop::update(float dt)
{
	for(auto itr = pipes_.begin(); itr!=pipes_.end(); ++itr)
	{
		itr->update(dt);
	}
}

const std::string& Loop::get_name() const
{
	return name_;
}