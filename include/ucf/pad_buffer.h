#pragma once

#include "hsd/gobj.h"
#include "melee/player.h"
#include "util/vector.h"
#include <gctypes.h>

constexpr auto UCF_PAD_BUFFER_SIZE = 4;
constexpr auto UCF_PAD_BUFFER_MASK = UCF_PAD_BUFFER_SIZE - 1;

struct ucf_pad_entry {
	vec2c stick;
};

struct shared_pad_buffer {
	struct alignas(4) {
		ucf_pad_entry entries[UCF_PAD_BUFFER_SIZE];
		u8 index;
	} buffer[4];
};

GAME_FUNC void PlayerThink_Input(HSD_GObj *gobj);

// Has to be after Slippi's restore point at PlayerThink_Input+0x3CC
#define UCF_PAD_BUFFER_INJECTION (PlayerThink_Input+0x410)

template<int offset>
inline const ucf_pad_entry &get_ucf_pad_buffer(int port)
{
	const auto *data = get_shared_data<shared_pad_buffer>(UCF_PAD_BUFFER_INJECTION);
	const auto &buffer = data->buffer[port];

	if constexpr (offset == 0)
		return buffer.entries[buffer.index];
	else
		return buffer.entries[(buffer.index + offset) & UCF_PAD_BUFFER_MASK];
}

// Tilt intent algorithm designed by tauKhan
constexpr auto UCF_TILT_THRESHOLD_SQR = 75 * 75;

inline bool check_ucf_xsmash(const Player *player)
{
	const auto &prev_input = get_ucf_pad_buffer<-2>(player->port);
	const auto &current_input = get_ucf_pad_buffer<0>(player->port);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > UCF_TILT_THRESHOLD_SQR;
}

inline bool check_ucf_sdi(const Player *player)
{
	const auto &prev_input = get_ucf_pad_buffer<-2>(player->port);
	const auto &current_input = get_ucf_pad_buffer<0>(player->port);
	const auto delta_x = current_input.stick.x - prev_input.stick.x;
	const auto delta_y = current_input.stick.y - prev_input.stick.y;
	return delta_x * delta_x + delta_y * delta_y > UCF_TILT_THRESHOLD_SQR;
}