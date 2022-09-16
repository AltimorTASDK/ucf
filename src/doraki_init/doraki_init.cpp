#include "hsd/gobj.h"
#include "melee/action_state.h"
#include "melee/player.h"
#include "ucf/doraki.h"

register HSD_GObj *gobj asm("r30");
register Player *player asm("r31");


static void gecko_entry()
{
	// Don't clobber this
	register float start_frame asm("fr1");

	// Record whether Fall was entered from CliffWait
	auto *as_data = player->custom_as_data<doraki_data>();
	as_data->ledgefall = player->action_state == AS_CliffWait;
	as_data->can_doraki = false;

	// Overwritten instruction (pass gobj to Player_ASChange)
	register HSD_GObj *gobj_in asm("r3");
	gobj_in = gobj;
	FORCE_WRITE(gobj_in);
}

GAME_FUNC void AS_029_Fall(HSD_GObj *gobj);

GECKO_INIT(AS_029_Fall+0xA0, gecko_entry);