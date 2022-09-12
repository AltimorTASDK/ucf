#include "hsd/asm/gobj.h"
#include "melee/asm/constants.h"
#include "melee/asm/player.h"

/*
 * Injection address
 */
.section .gecko.target, "a"
.int AS_235_SpotDodge_YoshiCheck+0x10

/*
 * Constants
 */
// Size of injected function's stack
.set TARGET_STACK_SIZE, 0x18

/*
 * Registers
 */
.set gpr.gobj,      3
.set gpr.player,    4
.set gpr.plco,      5
.set gpr.pic,       6
.set gpr.tmp,       7

.section .init
.global __init
__init:
        bl start

PIC:
// New spot dodge threshold when performing Axe method shield drop
y_threshold:   .float -.8000
// Integer coord corresponding to 1.0
rim_magnitude: .float 80
// Make normal coords round down to previous value during integer conversion so
// that the result is the same as the corresponding Nana coord
coord_bias:    .float 0.0001
// Temporary space to put fctiwz result in GPR
fctiwz_tmp_hi: .int   0
fctiwz_tmp_lo: .int   0

// Result in gpr.tmp
convert_coord_for_rim_check:
        // (int)(abs(x) * 80 - 0.0001) + 2
        fabs    f1, f1
        lfs     f2, rim_magnitude-PIC(gpr.pic)
        lfs     f3, coord_bias-PIC(gpr.pic)
        fmsubs  f1, f1, f2, f3
        fctiwz  f1, f1
        stfd    f1, fctiwz_tmp_hi-PIC(gpr.pic)
        lwz     gpr.tmp, fctiwz_tmp_lo-PIC(gpr.pic)
        addi    gpr.tmp, gpr.tmp, 2
        blr

start:
        // Overwritten instruction
        lwz     gpr.player, HSD_GObj.data(gpr.gobj)

        mflr    gpr.pic

        // Prioritize cstick spot dodge
        lwz     gpr.plco, plco-R13_BASE(r13)
        lfs     f0, Player.input.cstick.y(gpr.player)
        lfs     f1, PlCo.spot_dodge_stick_threshold(gpr.plco)
        fcmpo   cr0, f0, f1
        ble     end

        // Roll must be disabled
        lbz     r0, Player.input.stick_x_hold_time(gpr.player)
        lwz     gpr.tmp, PlCo.roll_stick_frames(gpr.plco)
        cmpw    r0, gpr.tmp
        blt     end

        // Must be above Y threshold
        lfs     f1, Player.input.stick.y(gpr.player)
        lfs     f0, y_threshold-PIC(gpr.pic)
        fcmpo   cr0, f1, f0
        ble     end

        // Must have valid ground (20XX save states don't restore this)
        lwz     r0, Player.phys.floor.line(gpr.player)
        cmpwi   r0, -1
        beq     end

        // Must be on a platform
        // groundCollLine[index].desc->type & LineType_Platform
        lwz     gpr.tmp, groundCollLine-R13_BASE(r13)
        rlwinm  r0, r0, 3, 0, 28
        lwzx    gpr.tmp, gpr.tmp, r0
        lhz     r0, CollisionLineDesc.type(gpr.tmp)
        rlwinm. r0, r0, 0, LineType_Platform
        beq     end

        // Rim coord check
        // (abs(x)+1)^2 + (abs(y)+1)^2 > 80^2
        // f1 is already set to stick y
        bl      convert_coord_for_rim_check
        mullw   r0, gpr.tmp, gpr.tmp
        lfs     f1, Player.input.stick.x(gpr.player)
        bl      convert_coord_for_rim_check
        mullw   gpr.tmp, gpr.tmp, gpr.tmp
        add     r0, r0, gpr.tmp
        cmpwi   r0, 80*80
        ble     end

        // Jump to 2 instructions ahead of return address.
        // This skips to "li r3, 0" in both call sites, making it as if the
        // spot dodge conditions failed. This allows UCF spot dodge suppression
        // to be implemented in a single injection despite the game having two
        // input checks.
        lwz     gpr.tmp, TARGET_STACK_SIZE+4(r1)
        addi    r1, r1, TARGET_STACK_SIZE
        addi    gpr.tmp, gpr.tmp, 8
        mtlr    gpr.tmp
        blr

end:
        // Continue into the spot dodge state
        nop