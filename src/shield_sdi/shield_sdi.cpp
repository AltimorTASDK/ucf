#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/powerpc.h"

inline bool check_ucf_shield_sdi(const Player *player)
{
	// Tilt intent algorithm designed by tauKhan
	const auto &prev_input = get_ucf_pad_buffer<-2>(player->port);
	const auto &current_input = get_ucf_pad_buffer<0>(player->port);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > 62 * 62;
}

static bool check_f2_sdi(const Player *player, const PlCo *plco)
{
	// Check that the player left the deadzone on the previous frame,
	// attempting a vanilla f2 SDI input but failing due to unlucky polling
	if (player->input.true_stick_x_hold_time >= 2)
		return false;

	// Check previous input to disallow buffering, and check for UCF 1f smash input
	const auto last_x = player->input.last_stick.x;
	const auto threshold = plco->sdi_stick_threshold;
	return last_x < threshold && check_ucf_shield_sdi(player);
}

static void gecko_entry()
{
	register Player *player asm("r4");
	register PlCo *plco asm("r5");

	register float stick_x asm("fr2");

	// Skip if vanilla check succeeded
	asm goto("cmpw   r3, r0\r\n"
	         "blt    %l[end]"
	         : : : : end);

	// This doesn't actually check for frame 2, but there's no need to as this
	// will fail whenever would the vanilla check would on any frame after
	u32 cr0;
	if (check_f2_sdi(player, plco))
		cr0 = ppc::cr<0>{.lt = 1, .gt = 0, .eq = 0}.raw;
	else
		cr0 = ppc::cr<0>{.lt = 0, .gt = 1, .eq = 1}.raw;

	asm("mtocrf 0b10000000, %0" : : "r"(cr0));
end:
	return;
}

GAME_FUNC void Player_ShieldSDICallback(HSD_GObj *gobj);

GECKO_INIT(Player_ShieldSDICallback+0x54, gecko_entry);
