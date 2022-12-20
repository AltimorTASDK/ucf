#include "hsd/pad.h"
#include "melee/player.h"
#include "ucf/pad_buffer.h"
#include "util/melee/pad.h"

SHARED_DATA shared_pad_buffer data;

register Player *player asm("r31");

inline bool check_ucf_sdrop(const port_pad_buffer &buffer)
{
	// Tilt intent algorithm designed by tauKhan
	const auto &prev_input = get_ucf_pad_buffer<-2>(buffer);
	const auto &current_input = get_ucf_pad_buffer<0>(buffer);
	const auto delta = current_input.stick.y - prev_input.stick.y;
	return delta * delta > 44 * 44;
}

static bool check_sdrop_up(const auto &buffer, const PlayerInput &input)
{
	// Must be -6125 or below along the rim, adjusted to prevent an ICs desync
	if (input.stick.y > FP(popo_to_nana(-.6125f)))
		return false;

	if (!is_rim_coord(input.stick))
		return false;

	// Only check speed on first frame
	if (buffer.sdrop_up_frames != 0)
		return true;

	return input.stick_y_hold_time < 2 && check_ucf_sdrop(buffer);
}

static void gecko_entry()
{
	if (!Player_IsCPU(player)) {
		auto *buffer = &data.buffer[player->port];

		// Save to next entry in ring buffer
		const auto status = get_input<0>(player->port);
		buffer->index = (buffer->index + 1) & UCF_PAD_BUFFER_MASK;
		buffer->entries[buffer->index].stick = status.stick;

		if (check_sdrop_up(*buffer, player->input))
			buffer->sdrop_up_frames++;
		else
			buffer->sdrop_up_frames = 0;
	}

	// Overwritten instruction
	register int analog_lr_neutral_time asm("r3");
	analog_lr_neutral_time = player->input.analog_lr_neutral_time;
	FORCE_WRITE(analog_lr_neutral_time);
}

GECKO_NAME("UCF Pad Buffer");
GECKO_INIT_PIC(UCF_PAD_BUFFER_INJECTION, gecko_entry, "r28");