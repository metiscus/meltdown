#pragma once

class Tank;

class Pump
{
	float max_flow_rate_;
	float flow_rate_;
	float mean_time_to_fail_;
	float powered_on_time_;
	bool is_powered_;
	bool is_failed_;
	bool is_flawless_;

	Tank *take_tank_;
	Tank *add_tank_;

public:
	Pump();

	void update(float dt);

	inline bool get_is_powered() const { return is_powered_; }
	inline bool get_is_failed() const { return is_failed_; }
	float get_flow_rate() const;

	void set_is_flawless(bool is_flawless) { is_flawless_ = is_flawless; }

	void set_is_powered(bool is_powered) { is_powered_ = is_powered; }
	void set_max_flow_rate(float rate) { max_flow_rate_ = rate; }
	void set_flow_rate(float rate) { flow_rate_ = rate; }
	void set_mean_time_to_fail(float rate) { mean_time_to_fail_ = rate; }

	void set_take_tank(Tank* tank) { take_tank_ = tank; }
	void set_add_tank(Tank* tank) { add_tank_ = tank; }

	inline void repair() { is_failed_ = false; powered_on_time_ = 0.0f; }
};