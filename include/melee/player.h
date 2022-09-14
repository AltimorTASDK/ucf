#pragma once

#include "hsd/gobj.h"
#include "melee/collision.h"
#include "util/vector.h"
#include <cmath>
#include <gctypes.h>

constexpr auto NANA_BUFFER = 30;

enum CID {
	CID_Mario,
	CID_Fox,
	CID_Falcon,
	CID_DK,
	CID_Kirby,
	CID_Bowser,
	CID_Link,
	CID_Sheik,
	CID_Ness,
	CID_Peach,
	CID_Popo,
	CID_Nana,
	CID_Pikachu,
	CID_Samus,
	CID_Yoshi,
	CID_Puff,
	CID_Mewtwo,
	CID_Luigi,
	CID_Marth,
	CID_Zelda,
	CID_YLink,
	CID_DrMario,
	CID_Falco,
	CID_Pichu,
	CID_GameNWatch,
	CID_Ganondorf,
	CID_Roy,
	CID_MasterHand,
	CID_CrazyHand,
	CID_MaleWire,
	CID_FemaleWire,
	CID_GigaBowser,
	CID_Sandbag,
	CID_Max
};

struct PlayerInput {
    vec2 stick;
    vec2 last_stick;
    vec2 frozen_stick;
    vec2 cstick;
    vec2 last_cstick;
    vec2 frozen_cstick;
    f32 analog_lr;
    f32 last_analog_lr;
    f32 frozen_analog_lr;
    u32 held_buttons;
    u32 last_held_buttons;
    u32 frozen_buttons;
    u32 instant_buttons;
    u32 released_buttons;
    u8 stick_x_hold_time;
    u8 stick_y_hold_time;
    u8 analog_lr_hold_time;
    u8 true_stick_x_hold_time;
    u8 true_stick_y_hold_time;
    u8 true_analog_lr_hold_time;
    u8 stick_x_neutral_time;
    u8 stick_y_neutral_time;
    u8 analog_lr_neutral_time;
    u8 stick_x_direction_hold_time;
    u8 stick_y_direction_hold_time;
    u8 analog_lr_direction_hold_time;
    u8 last_a_press;
    u8 last_b_press;
    u8 last_xy_press;
    u8 last_analog_lr_press;
    u8 last_digital_lr_press;
    u8 last_dpad_down_press;
    u8 last_dpad_up_press;
    u8 a_press_interval;
    u8 digital_lr_press_interval;
    u8 last_jump_input;
    u8 last_up_b_input;
    u8 last_down_b_input;
    u8 last_side_b_input;
    u8 last_neutral_b_input;
    u8 jump_input_interval;
    u8 up_b_input_interval;
};

struct PopoData {
	u32 buttons;
	u8 analog_l;
	u8 analog_r;
	vec2c stick;
	vec2c cstick;
	vec3 position;
	f32 direction;
};

struct CPUData {
	char pad000[0xFC];
	PopoData popo_data_buffer[NANA_BUFFER];
	PopoData *popo_data_write;
	PopoData *popo_data_read;
	char pad44C[0x558 - 0x44C];
};

struct Player {
	HSD_GObj *gobj;
	u32 character_id;
	u32 spawn_count;
	u8 slot;
	u32 action_state;
	char pad0009[0x2C - 0x14];
	f32 direction;
	char pad0030[0x618 - 0x30];
	u8 port;
	char pad0619[0x620 - 0x619];
	PlayerInput input;
	char pad068C[0x6F0 - 0x68C];
	Physics phys;
	s32 ecb_timer;
	void *camera_data;
	f32 animation_frame;
	f32 subaction_speed;
	f32 animation_speed;
	char pad08A0[0x1A88 - 0x8A0];
	CPUData cpu;
	char pad1FE0[0x221F - 0x1FE0];
	struct {
#ifdef PAL
		u8 flags8_80 : 1;
		u8 flags8_40 : 1;
		u8 flags8_20 : 1;
		u8 flags8_10 : 1;
		u8 flags8_08 : 1;
		u8 no_update : 1;
		u8 is_secondary_char : 1;
		u8 flags8_01 : 1;
#else
		u8 flags8_80 : 1;
		u8 flags8_40 : 1;
		u8 flags8_20 : 1;
		u8 no_update : 1;
		u8 is_secondary_char : 1;
		u8 flags8_04 : 1;
		u8 flags8_02 : 1;
		u8 flags8_01 : 1;
#endif
	};
	char pad2220[0x2340 - 0x2220];
	union {
		char raw[0xAC];
		struct {
			u32 is_smash_turn;
			f32 turn_direction;
			f32 dash_direction;
			u32 pad00C;
			f32 tilt_turn_timer;
			u32 pad014;
			u32 can_dash;
			u32 buffered_buttons;
		} Turn;
	} as_data;

	bool is_facing_left() const
	{
		// Enable optimizations by disregarding negative zero
		return std::signbit(direction);
	}

	// Place new data at end of AS data space
	template<typename T>
	T *custom_as_data()
	{
		return (T*)&as_data.raw[sizeof(as_data.raw) - sizeof(T)];
	}

	template<typename T>
	const T *custom_as_data() const
	{
		return (T*)&as_data.raw[sizeof(as_data.raw) - sizeof(T)];
	}
};

static_assert(sizeof(Player) == 0x23EC);

GAME_FUNC HSD_GObj *PlayerBlock_GetSubCharGObj(s32 slot, s32 subchar);
GAME_FUNC bool Player_IsCPU(const Player *player);