#include "hsd/pad.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/melee/pad.h"

SHARED_DATA shared_pad_buffer data;

register Player *player asm("r31");

static void gecko_entry()
{
	if (!Player_IsCPU(player)) {
		auto *buffer = &data.buffer[player->port];

		// Save to next entry in ring buffer
		const auto status = get_input<0>(player->port);
		buffer->index = (buffer->index + 1) & UCF_PAD_BUFFER_MASK;
		buffer->entries[buffer->index].stick = status.stick;
	}

	// Overwritten instruction
	register int stick_x_neutral_time asm("r3");
	stick_x_neutral_time = player->input.stick_x_neutral_time;
	FORCE_WRITE(stick_x_neutral_time);
}

GECKO_NO_STACK_FRAME();
GECKO_INIT_PIC(UCF_PAD_BUFFER_INJECTION, gecko_entry, "r28");