#pragma once

#include "hsd/pad.h"
#include "melee/player.h"

template<int offset> requires (offset <= 0 && offset >= -PAD_QNUM + 1)
inline const PADStatus &get_input(int port)
{
	auto index = HSD_PadLibData.qread + offset - 1;
	if (index < 0)
		index += PAD_QNUM;

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