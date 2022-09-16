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

inline int abs_coord_to_int(float x)
{
	// Small bias converts Nana coords back to the corresponding Popo coords
	return (int)(std::abs(x) * FP(80.f) - FP(0.0001f)) + 1;
}

inline bool is_rim_coord(const vec2 &coords)
{
	const auto converted = vec2i(abs_coord_to_int(coords.x) + 1,
	                             abs_coord_to_int(coords.y) + 1);
	return converted.length_sqr() > 80 * 80;
}

constexpr float popo_to_nana(float x)
{
	return x >= 0 ? (float)((s8)(x * 127)) / 127 : (float)((s8)(x * 128)) / 128;
}