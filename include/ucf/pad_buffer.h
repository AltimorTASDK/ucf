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

struct alignas(4) port_pad_buffer {
	ucf_pad_entry entries[UCF_PAD_BUFFER_SIZE];
	u8 index;
	u8 sdrop_up_frames;
};

struct shared_pad_buffer {
	port_pad_buffer buffer[4];
};

GAME_FUNC void PlayerThink_Input(HSD_GObj *gobj);

// Has to be after Slippi's restore point at PlayerThink_Input+0x3CC
#define UCF_PAD_BUFFER_INJECTION (PlayerThink_Input+0x750)

inline const auto &get_ucf_pad_data(int port)
{
	return get_shared_data<shared_pad_buffer>(UCF_PAD_BUFFER_INJECTION)->buffer[port];
}

template<int offset>
inline const ucf_pad_entry &get_ucf_pad_buffer(const port_pad_buffer &buffer)
{
	if constexpr (offset == 0)
		return buffer.entries[buffer.index];
	else
		return buffer.entries[(buffer.index + offset) & UCF_PAD_BUFFER_MASK];
}

template<int offset>
inline const ucf_pad_entry &get_ucf_pad_buffer(int port)
{
	return get_ucf_pad_buffer<offset>(get_ucf_pad_data(port));
}

inline bool check_ucf_xsmash(const Player *player)
{
	// Tilt intent algorithm designed by tauKhan
	const auto &prev_input = get_ucf_pad_buffer<-2>(player->port);
	const auto &current_input = get_ucf_pad_buffer<0>(player->port);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > 75 * 75;
}