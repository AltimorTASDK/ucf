#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

register HSD_GObj *gobj asm("r31");

static bool check_sdrop_extension(const vec2 &stick)
{
	// Must be a rim coord
	return stick.y <= FP(popo_to_nana(-.6125f)) && is_rim_coord(stick);
}

static void gecko_entry()
{
	register bool success asm("r0");
	register Player *player asm("r4");

	success = player->input.stick_y_hold_time < 3 &&
	          check_sdrop_extension(player->input.stick) &&
	          check_sdrop_extension(player->input.last_stick);

	FORCE_WRITE(success);
}

GAME_FUNC bool Interrupt_Pass_OoS(HSD_GObj *gobj);

GECKO_INIT_PIC(Interrupt_Pass_OoS+0x80, gecko_entry, "r30")