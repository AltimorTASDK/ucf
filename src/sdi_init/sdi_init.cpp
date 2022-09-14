#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/sdi.h"

register Player *player asm("r29");

static void gecko_entry()
{
	register HSD_GObj *gobj asm("r3");

	// Overwritten instruction
	player = gobj->get<Player>();

	// Don't enable buffering of SDI
	const auto magnitude_sqr = player->input.stick.length_sqr();
	const auto threshold_sqr = plco->sdi_stick_threshold * plco->sdi_stick_threshold;
	player->custom_as_data<sdi_data>()->allow_f2_sdi = magnitude_sqr < threshold_sqr;
}

GAME_FUNC void Player_EnterDamageState(HSD_GObj *gobj, u32 state, f32 direction);

GECKO_INIT(Player_EnterDamageState+0x38, gecko_entry);