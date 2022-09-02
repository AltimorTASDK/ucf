#pragma once

#include "hsd/pad.h"
#include "melee/player.h"

template<int offset>
inline const PADStatus &get_input(int port)
{
	// Modulo the offset at compile time
	constexpr auto mod_offset = (((offset - 1) % PAD_QNUM) + PAD_QNUM) % PAD_QNUM;

	// Only need a compare at runtime
	auto index = HSD_PadLibData.qread + mod_offset;
	if (index >= PAD_QNUM)
		index -= PAD_QNUM;

	return HSD_PadLibData.queue[index].stat[port];
}

inline bool check_ucf_xsmash(const Player *player)
{
	// Tilt intent algorithm designed by tauKhan
	const auto &prev_input = get_input<-2>(player->port);
	const auto &current_input = get_input<0>(player->port);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > 75 * 75;
}