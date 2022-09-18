#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/powerpc.h"

inline bool check_ucf_sdi(const Player *player)
{
	// Tilt intent algorithm designed by tauKhan
	const auto &prev_input = get_ucf_pad_buffer<-2>(player->port);
	const auto &current_input = get_ucf_pad_buffer<0>(player->port);
	const auto delta_x = current_input.stick.x - prev_input.stick.x;
	const auto delta_y = current_input.stick.y - prev_input.stick.y;
	return delta_x * delta_x + delta_y * delta_y > 75 * 75;
}

static bool check_f2_sdi(const Player *player, const PlCo *plco)
{
	// Check that the player left the deadzone on the previous frame,
	// attempting a vanilla f2 SDI input but failing due to unlucky polling
	if (player->input.true_stick_x_hold_time >= 2 && player->input.true_stick_y_hold_time >= 2)
		return false;

	// Check previous input to disallow buffering, and check for UCF 1f smash input
	const auto magnitude_sqr = player->input.last_stick.length_sqr();
	const auto threshold_sqr = plco->sdi_stick_threshold * plco->sdi_stick_threshold;
	return magnitude_sqr < threshold_sqr && check_ucf_sdi(player);
}

static void gecko_entry()
{
	register Player *player asm("r3");
	register PlCo *plco asm("r4");

	register float stick_x asm("fr3");
	register float stick_y asm("fr2");

	// Skip if vanilla check succeeded
	asm goto("cmpw   r0, r5\r\n"
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

GAME_FUNC void Player_SDICallback(HSD_GObj *gobj);

GECKO_INIT(Player_SDICallback+0x5C, gecko_entry);
