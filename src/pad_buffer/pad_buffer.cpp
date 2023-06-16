#include "hsd/pad.h"
#include "melee/player.h"
#include "melee/characters/zelda.h"
#include "ucf/pad_buffer.h"
#include "util/melee/pad.h"
#include <bit>
#include <cmath>

SHARED_DATA shared_pad_buffer data;

register Player *player asm("r31");

static bool check_ucf_sdrop(const port_pad_buffer &buffer)
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

[[gnu::noinline]] static void apply_cardinals(const vec2c &raw_stick, vec2 *stick)
{
	// Produce 1.0 cardinals when one axis is >= 80 and
	// the other is within [-SNAP_RANGE, SNAP_RANGE]
	constexpr auto SNAP_RANGE = 6;

	const auto raw_x = raw_stick.x;
	const auto raw_y = raw_stick.y;

	if ((raw_x <= -80 || raw_x >= 80) && raw_y >= -SNAP_RANGE && raw_y <= SNAP_RANGE)
		*(vec2i*)stick = {std::bit_cast<int>(1.f) ^ (std::signbit(raw_stick.x) << 31), 0};
	else if ((raw_y <= -80 || raw_y >= 80) && raw_x >= -SNAP_RANGE && raw_x <= SNAP_RANGE)
		*(vec2i*)stick = {0, std::bit_cast<int>(1.f) ^ (std::signbit(raw_stick.y) << 31)};
}

static bool should_apply_cardinals(const Player *player)
{
	switch (player->character_id) {
	case CID_Zelda:
		return player->action_state != AS_Zelda_SpecialHiStart;
	default:
		return true;
	}
}

static void gecko_entry()
{
	if (!Player_IsCPU(player)) {
		auto *buffer = &data.buffer[player->port];

		// Save to next entry in ring buffer
		const auto status = get_input<0>(player->port);
		buffer->index = (buffer->index + 1) & UCF_PAD_BUFFER_MASK;
		buffer->entries[buffer->index].stick = status.stick;

		if (should_apply_cardinals(player)) {
			apply_cardinals(status.stick, &player->input.stick);
			apply_cardinals(status.cstick, &player->input.cstick);
		}

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