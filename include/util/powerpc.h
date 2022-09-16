#pragma once

#include <gctypes.h>

namespace ppc {

// Condition register fields for crN
template<int N>
union cr {
	struct {
		u32 : 4 * N;
		u32 lt : 1;
		u32 gt : 1;
		u32 eq : 1;
		u32 so : 1;
	};
	u32 raw;
};

} // namespace ppc