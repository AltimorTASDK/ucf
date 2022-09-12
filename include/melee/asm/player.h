#pragma once

#include "melee/asm/collision.h"

/*
 * Structs
 */
.set Player.input.stick.x,           0x620
.set Player.input.stick.y,           0x624
.set Player.input.cstick.x,          0x638
.set Player.input.cstick.y,          0x63C
.set Player.input.stick_x_hold_time, 0x670
.set Player.input.stick_y_hold_time, 0x671
.set Player.phys,                    0x6F0
.set Player.phys.floor.line,         Player.phys + Physics.floor.line