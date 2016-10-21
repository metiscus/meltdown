#include "reactor.h"
#include <iostream>
#include <SDL.h>
#include <cassert>
#include <cstdio>
#include "renderer_gl.h"
#include "color.h"

int time_scale = 30;

void draw_reactor(Reactor& reactor, RenderInterface& renderer);
int main(int argc, char** argv)
{
	RendererGL renderer;
	renderer.create_window(1024, 768, "Meltdown");
	renderer.load_font("font.ttf", {10, 12, 14});

	Reactor reactor;

	for(int ii=0; ii<3000; ++ii)
	{
		renderer.begin();
		for(int st = 0; st <time_scale; ++st)
		{
			reactor.update(1.0/60.0);	
		}
		
		draw_reactor(reactor, renderer);
		renderer.end();
	}

	return 0;
}

void draw_reactor(Reactor& reactor, RenderInterface& renderer)
{
	auto bright_green      = Color::FromRGBA(60, 200, 60, 255);
	auto bright_red        = Color::FromRGBA(200, 60, 60, 255);
	auto bright_yellow     = Color::FromRGBA(200, 200, 60, 255);
	auto light_gray        = Color::FromRGBA(200, 200, 200, 255);
	auto light_blue        = Color::FromRGBA(170, 170, 200, 255);
	auto light_blue_trans  = Color::FromRGBA(170, 170, 200, 128);
	auto med_blue          = Color::FromRGBA(80, 80, 180, 255);
	auto dark_gray         = Color::FromRGBA(50, 50, 50, 255);
	auto med_gray          = Color::FromRGBA(120, 120, 120, 255);

	auto pump_map = reactor.get_pump_map();

	// draw containment building
	renderer.draw_rectangle(light_gray, 50, 50, 500, 500, false);
	renderer.draw_rectangle(med_gray, 50, 50, 500, 500, true, 1);

	// draw the reactor containment vessel
	int containment_height = 200;
	renderer.draw_rectangle(light_gray, 100, 100, 200, 100+containment_height, true);
	renderer.draw_rectangle(dark_gray, 100, 100, 200, 100+containment_height, false);
	float fluid_coverage = reactor.get_core_fluid_fill_percent();
	std::cerr<<"Fluid coverage: "<<fluid_coverage<<"\n";
	renderer.draw_rectangle(light_blue_trans, 100, 100, 200, 100+containment_height*fluid_coverage, true, -5);

	// draw the reactor core
	renderer.draw_rectangle(bright_red, 120, 120, 180, 280, true, -1);
	renderer.draw_rectangle(dark_gray, 100, 100, 200, 300, false, -1);
	
	// draw the pressurizer
	renderer.draw_rectangle(dark_gray, 240, 200, 280, 300, false, -1);
	renderer.draw_rectangle(light_blue, 240, 200, 280, 300, true);

	// draw the steam generator
	renderer.draw_rectangle(dark_gray, 320, 100, 400, 300, false, -1);
	renderer.draw_rectangle(light_blue, 320, 100, 400, 300, true);

	// draw the reactor coolant pump
	auto pump = pump_map["Reactor Coolant"];
	if(pump)
	{
		if(pump->get_is_powered() && !pump->get_is_failed())
		{
			renderer.draw_rectangle(bright_green, 250, 100, 300, 120, false, -3);
		}
		else if(pump->get_is_powered() && pump->get_is_failed())
		{
			renderer.draw_rectangle(bright_red, 250, 100, 300, 120, false, -3);
			time_scale = 1.0;
		}
		else
		{
			renderer.draw_rectangle(bright_yellow, 250, 100, 300, 120, false, -3);
		}
		
	}
	renderer.draw_rectangle(light_gray, 250, 100, 300, 120, true, -2);
	
	
	// draw the pipe from the steam generator to the reactor
	renderer.draw_rectangle(med_blue, 200, 280, 320, 300, true, 0);

	// draw the pipe from the reactor to the steam generator
	renderer.draw_rectangle(med_blue, 200, 100, 320, 120, true, 0);

	// draw the pipe from the reactor to the steam generator
	renderer.draw_rectangle(med_blue, 200, 100, 320, 120, true, 0);

	// draw a label
	renderer.draw_text("Containment Building", 14, 20, 20, -9);

	renderer.draw_text(reactor.get_date_string().c_str(), 14, 500, 700, -9);
}