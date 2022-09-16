#pragma once

#include "util/vector.h"
#include <gctypes.h>

// Custom AS data
struct doraki_data {
	// Whether a ledgefall was performed this frame
	bool ledgefall;
	// Whether the player was eligible to doraki on f1 of Fall
	bool can_doraki;
	// Position/ECB flags when player was originally eligible to doraki
	vec3 doraki_position;
	u32 doraki_ecb_flags;
};