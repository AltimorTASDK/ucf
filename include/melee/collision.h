#pragma once

#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "util/vector.h"
#include <gctypes.h>

enum Collide {
	Collide_LeftWallHug  = 0x20,
	Collide_RightWallHug = 0x800
};

enum LineType {
	LineType_Platform = 0x100
};

enum ECBFlag {
	ECBFlag_FreezeBottom = 0x10
};

struct CollisionLineDesc {
	u16 points[2];
	u16 connected_lines[4];
	u16 flags;
	u16 type;
};

struct CollisionLine {
	CollisionLineDesc *desc;
	u32 flags;
};

struct ECB {
	vec2 top, bottom, left, right;
};

struct CollisionContact {
	s32 line;
	u32 type;
	vec3 normal;
};

struct Physics {
	HSD_GObj *gobj;
	vec3 position;
	vec3 trace_start_position;
	vec3 start_position;
	vec3 last_position;
	char pad0034[0x84 - 0x34];
	ECB desired_ecb;
	ECB ecb;
	ECB trace_start_ecb;
	ECB last_ecb;
	s32 use_ecb_bones;
	HSD_JObj *root_bone;
	HSD_JObj *ecb_bones[6];
	char pad0124[0x130 - 0x124];
	u32 ecb_flags;
	u32 collide;
	u32 last_collide;
	char pad013C[0x140 - 0x13C];
	vec3 contact_point;
	CollisionContact floor;
	CollisionContact left_wall;
	CollisionContact right_wall;
	CollisionContact ceiling;
};

GAME_FUNC u32 Physics_IsOnPlatform(const Physics *phys);

GAME_SDATA CollisionLine *groundCollLine;