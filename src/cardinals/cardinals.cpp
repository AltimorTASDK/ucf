#include "hsd/pad.h"

GECKO_NO_STACK_FRAME();

register int pad_index asm("r21");
register PADStatus *volatile status asm("r31");

[[gnu::noinline]] static void apply_cardinals(vec2c *stick)
{
	// Produce 1.0 cardinals when one axis is >= 80 and
	// the other is within [-SNAP_RANGE, SNAP_RANGE]
	constexpr auto SNAP_RANGE = 6;

	const auto abs_x = std::abs(stick->x);
	const auto abs_y = std::abs(stick->y);

	if (abs_x >= 80 && abs_y <= SNAP_RANGE)
		stick->y = 0;
	else if (abs_y >= 80 && abs_x <= SNAP_RANGE)
		stick->x = 0;
}

static void gecko_entry()
{
	apply_cardinals(&status->stick);
	apply_cardinals(&status->cstick);

	// Overwritten instruction
	pad_index++;
}

GAME_FUNC u32 PADRead(PADStatus *status);

GECKO_INIT(PADRead+0x378, gecko_entry);