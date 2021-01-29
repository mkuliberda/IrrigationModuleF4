#include "hysteresis.h"


void Hysteresis::set_hysteresis_time_from(const bool _from_state, const double _new_hysteresis_time_ms)
{
	if (_from_state) {
		time_from_true_ms = _new_hysteresis_time_ms;

	}
	else {
		time_from_false_ms = _new_hysteresis_time_ms;
	}
}

void Hysteresis::set_state_and_update(const bool _new_state, const double& _now_ms)
{
	if (_new_state != state) {
		if (_new_state != requested_state) {
			requested_state = _new_state;
			last_time_to_change_state = _now_ms;
		}

	}
	else {
		requested_state = state;
	}

	update(_now_ms);
}

void Hysteresis::update(const double& _now_ms)
{
	if (requested_state != state) {

		const double elapsed = _now_ms - last_time_to_change_state;

		if (state && !requested_state) {
			// true -> false
			if (elapsed >= time_from_true_ms) {
				state = false;
			}

		}
		else if (!state && requested_state) {
			// false -> true
			if (elapsed >= time_from_false_ms) {
				state = true;
			}
		}
	}
}
