#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct PADStatus {
	u16 buttons;
	vec2c stick;
	vec2c cstick;
	u8 analog_l;
	u8 analog_r;
	u8 analog_a;
	u8 analog_b;
	u8 err;
};