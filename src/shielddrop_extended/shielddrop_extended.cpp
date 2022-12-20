#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/melee/pad.h"

register HSD_GObj *gobj asm("r31");

static void gecko_entry()
{
	register Player *player asm("r4");
	register PlCo   *plco   asm("r5");

	// Skip if vanilla check succeeded
	asm goto("cror   eq, lt, eq\r\n"
	         "beq    %l[end]"
	         : : : : end);

	// Require 2f of input so sdrop doesn't trigger early on the way down
	if (get_ucf_pad_data(player->port).sdrop_up_frames >= 2)
		asm("crset eq");
	else
		asm("crclr eq");
end:
	return;
}

GAME_FUNC bool Interrupt_Pass_OoS(HSD_GObj *gobj);

GECKO_INIT(Interrupt_Pass_OoS+0x38, gecko_entry);