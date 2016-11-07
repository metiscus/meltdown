#pragma once
#include <string>
#include <vector>

class Pipe;

class Loop
{
	std::string name_;
	std::vector<Pipe> pipes_;

public:
	Loop(const std::string& name);

	void  add_pipe(Pipe pipe);
	Pipe* get_pipe(const std::string& id);

	void  update(float dt);

	const std::string& get_name() const;
};