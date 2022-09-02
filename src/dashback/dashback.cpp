#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

register HSD_GObj *gobj asm("r30");
register Player *player asm("r31");

static void gecko_entry()
{
	// Overwritten instruction
	register float new_direction asm("32");
	player->direction = new_direction;

	// No Nana
	if (player->is_secondary_char)
		return;

	// Must be tilt turn f2
	if (player->animation_frame != FP(2.f))
		return;

	// Must satisfy vanilla xsmash conditions
	if (player->input.stick.x * new_direction < plco->x_smash_threshold)
		return;

	if (player->input.stick_x_hold_time >= 2)
		return;

	if (!check_ucf_xsmash(player))
		return;

	player->as_data.Turn.is_smash_turn = true;
	player->as_data.Turn.can_dash = true;

	// Retroactively write dashback to Nana inputs
	auto *nana_gobj = PlayerBlock_GetSubCharGObj(player->slot, 1);
	if (nana_gobj == nullptr)
		return;

	auto *nana = nana_gobj->get<Player>();
	nana->cpu.popo_data_write->direction = player->direction;
	// -128 or 127 (gcc won't do this automatically on -Os)
	nana->cpu.popo_data_write->stick.x = (s8)(127 + player->is_facing_left());
}

GAME_FUNC void Interrupt_AS_Turn(HSD_GObj *gobj);

GECKO_INIT(Interrupt_AS_Turn+0x4C, gecko_entry, "r29")