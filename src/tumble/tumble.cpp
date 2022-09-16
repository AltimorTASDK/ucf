#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/powerpc.h"

register HSD_GObj *gobj asm("r30");
register Player *player asm("r31");

static void gecko_entry()
{
	register u8 stick_x_hold_time asm("r3");
	register f32 wiggle_threshold asm("fr0");

	// Skip if vanilla check succeeded
	// Skip if more than 2f input and take bge
	asm goto("cmpwi  %0, 1\r\n"
	         "bne    %l[end]"
	         : : "r"(stick_x_hold_time) : : end);

	// Check previous input to disallow buffering, and check for UCF 1f smash input
	u32 cr0;
	if (std::abs(player->input.last_stick.x) < wiggle_threshold && check_ucf_xsmash(player))
		cr0 = ppc::cr<0>{.lt = 1, .gt = 0, .eq = 0}.raw;
	else
		cr0 = ppc::cr<0>{.lt = 0, .gt = 1, .eq = 1}.raw;

	asm("mtocrf 0b10000000, %0" : : "r"(cr0));
end:
	return;
}

GAME_FUNC void Interrupt_AS_DamageFall(HSD_GObj *gobj);

GECKO_INIT(Interrupt_AS_DamageFall+0xCC, gecko_entry);
