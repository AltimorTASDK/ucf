#include "dolphin/memory.h"
#include "hsd/gobj.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/doraki.h"
#include "ucf/pad_buffer.h"

register HSD_GObj *gobj asm("r28");
register Player *player asm("r30");

GAME_FUNC bool Interrupt_Walljump(HSD_GObj *gobj);

static bool check_doraki_input(const Player *player)
{
	// Must be pressing away from wall
	if (player->input.stick.x * -player->walljump_direction < plco->walljump_stick_threshold)
		return false;

	if (player->input.stick_x_hold_time >= 2)
		return false;

	// Hardware value comparison
	return check_ucf_xsmash(player);
}

static bool attempt_doraki(Player *player)
{
	const auto *as_data = player->custom_as_data<doraki_data>();

	// The player missed the f1 smash input and drifted away, allow an f2 doraki
	const auto position = player->position;
	const auto collide = player->phys.collide;
	const auto ecb = player->phys.ecb;
	player->position = as_data->doraki_position;
	player->phys.collide = player->phys.last_collide;
	player->phys.ecb = player->phys.last_ecb;

	if (Interrupt_Walljump(player->gobj))
		return true;

	// Walljump failed, restore position/collide/ECB
	player->position = position;
	player->phys.collide = collide;
	player->phys.ecb = ecb;
	return false;
}

static void gecko_entry()
{
	// Overwrites call to Interrupt_Walljump
	constexpr auto wall_hug_mask = Collide_LeftWallHug | Collide_RightWallHug;
	auto *as_data = player->custom_as_data<doraki_data>();

	if (player->action_state != AS_Fall)
		GECKO_RETURN(Interrupt_Walljump(gobj));

	if (as_data->can_doraki) {
		as_data->can_doraki = false;

		if ((player->phys.collide & wall_hug_mask) == 0 && check_doraki_input(player))
			GECKO_RETURN(attempt_doraki(player));
	}

	if (Interrupt_Walljump(gobj))
		GECKO_RETURN(true);

	if (as_data->ledgefall) {
		// Store doraki eligibility for f2
		as_data->ledgefall = false;
		as_data->can_doraki = player->walljump_eligible_frames < 0xFE;
		if (as_data->can_doraki) {
			as_data->doraki_position = player->position;
			as_data->doraki_ecb_flags = player->phys.ecb_flags;
		}
	}

	GECKO_RETURN(false);
}

GAME_FUNC void Player_CollisionAirCanGrabLedgeAndWalljumpPlatformPass(HSD_GObj *gobj);

GECKO_INIT(Player_CollisionAirCanGrabLedgeAndWalljumpPlatformPass+0x120, gecko_entry);