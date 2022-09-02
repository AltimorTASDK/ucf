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

	if (Player_IsCPU(player))
		return;

	// Must be tilt turn f2
	if (player->animation_frame != FP(2.f))
		return;

	if (player->as_data.Turn.is_smash_turn)
		return;

	// Must satisfy vanilla xsmash conditions
	// UCF v0.74+ doesn't check direction and it allows a UCF only ICs desync
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
	nana->cpu.popo_data_write->direction = new_direction;
	if (new_direction > FP(0.f))
		nana->cpu.popo_data_write->stick.x = 127;
	else
		nana->cpu.popo_data_write->stick.x = -128;

	nana->cpu.popo_data_write->stick.y = 0;
}

GAME_FUNC void Interrupt_AS_Turn(HSD_GObj *gobj);

GECKO_INIT(Interrupt_AS_Turn+0x4C, gecko_entry, "r29")