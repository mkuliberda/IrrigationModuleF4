#pragma once
#include <cmath>

inline double operator"" _sec(long double sec) {
	return sec;
}

inline double operator"" _msec(long double msec) {
	return msec / 1000.0;
}

inline double operator"" _min(long double min) {
	return min * 60.0;
}

inline double operator"" _h(long double h) {
	return h * 60.0 * 60.0;
}

inline bool double_equals(double a, double b, double epsilon = 0.05) {
	return std::abs(a - b) < epsilon;
}

class Hysteresis
{
public:
	Hysteresis() = default;

	~Hysteresis() = default;

	//To avoid runtime errors, delete copy constructor and copy assignment operator. If sth's wrong, compile time error will fire.
	Hysteresis(Hysteresis const &) = delete;
	Hysteresis& operator=(Hysteresis const&) = delete;


	bool get_state() const { return state; }

	void set_hysteresis_time_from(const bool _from_state, const double _new_hysteresis_time_ms);

	void set_state_and_update(const bool _new_state, const double& _now_ms);

	void update(const double& _now_ms);

private:

	double last_time_to_change_state{ 0 };

	double time_from_true_ms{ 0 };
	double time_from_false_ms{ 0 };

	bool state{ false };
	bool requested_state{ false };
};
