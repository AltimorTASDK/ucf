#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/sdi.h"
#include "util/powerpc.h"

static bool check_f2_sdi(const Player *player, const PlCo *plco)
{
	// Check previous input to disallow buffering
	const auto magnitude_sqr = player->input.last_stick.length_sqr();
	const auto threshold_sqr = plco->sdi_stick_threshold * plco->sdi_stick_threshold;
	// Check that the player left the deadzone on the previous frame,
	// attempting a vanilla f2 SDI input but failing due to unlucky polling
	return magnitude_sqr < threshold_sqr && (player->input.true_stick_x_hold_time < 2 ||
	                                         player->input.true_stick_y_hold_time < 2);
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
	if (check_f2_sdi(player, plco))
		asm("crset lt");
	else
		asm("crclr lt");
end:
	return;
}

GAME_FUNC void Player_SDICallback(HSD_GObj *gobj);

GECKO_INIT(Player_SDICallback+0x5C, gecko_entry);
