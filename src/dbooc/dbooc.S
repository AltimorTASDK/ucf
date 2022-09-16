#include "hsd/asm/gobj.h"
#include "melee/asm/constants.h"
#include "melee/asm/player.h"

/*
 * Injection address
 */
.section .gecko.target, "a"
.int Interrupt_SquatRv+0x14

/*
 * Registers
 */
.set gpr.plco,   4
.set gpr.pic,    4
.set gpr.player, 5
.set gpr.tmp,    6

.section .init
.global __init
__init:
        bl start

PIC:
// Raise the max SquatWait coord by 2 values.
// This would be 6000, but 5900 avoids an ICs desync.
new_threshold: .float .5900
// Integer coord corresponding to 1.0
rim_magnitude: .float 80
// Make normal coords round down to previous value during integer conversion so
// that the result is the same as the corresponding Nana coord
coord_bias:    .float 0.0001
// Temporary space to put fctiwz result in GPR
fctiwz_tmp_hi: .int   0
fctiwz_tmp_lo: .int   0

//  Result in gpr.tmp
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
        lfs     f0, PlCo.max_squatwait_threshold(gpr.plco)

        mflr    gpr.pic

        // Must be outside deadzone for 1f (intending to dash on next frame)
        lbz     gpr.tmp, Player.input.stick_x_hold_time(gpr.player)
        cmpwi   gpr.tmp, 1
        bge     end

        // Rim coord check
        // (abs(x)+1)^2 + (abs(y)+1)^2 > 80^2
        lfs     f1, Player.input.stick.x(gpr.player)
        bl      convert_coord_for_rim_check
        mullw   r0, gpr.tmp, gpr.tmp
        lfs     f1, Player.input.stick.y(gpr.player)
        bl      convert_coord_for_rim_check
        mullw   gpr.tmp, gpr.tmp, gpr.tmp
        add     r0, r0, gpr.tmp
        cmpwi   r0, 80*80
        ble     end

        // Raise the threshold to cover the dreaded coordinates
        lfs     f0, new_threshold-PIC(gpr.pic)

end:
        nop