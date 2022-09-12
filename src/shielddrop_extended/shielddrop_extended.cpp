#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

register HSD_GObj *gobj asm("r31");

[[gnu::noinline]] static bool check_coord(const vec2 &stick)
{
	// Must be -6125 or below along the rim, adjusted to prevent an ICs desync
	return stick.y <= FP(popo_to_nana(-.6125f)) && is_rim_coord(stick);
}

static bool check_sdrop_extension(const Player *player)
{
	return player->input.stick_y_hold_time < 3 &&
	       check_coord(player->input.stick) &&
	       check_coord(player->input.last_stick);
}

static void gecko_entry()
{
	// Skip if vanilla check succeeded
	asm goto("cror   eq, lt, eq\r\n"
	         "beq    %l[end]"
	         : : : : end);

	register Player *player asm("r4");

	if (check_sdrop_extension(player))
		asm("crset eq");
	else
		asm("crclr eq");

end:
	return;
}

GAME_FUNC bool Interrupt_Pass_OoS(HSD_GObj *gobj);

GECKO_INIT_PIC(Interrupt_Pass_OoS+0x38, gecko_entry, "r30");