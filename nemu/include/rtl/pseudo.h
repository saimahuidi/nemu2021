#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#include "rtl/rtl.h"
#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  rtl_xori(s, dest, src1, -1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  // move 0 to t0
  rtl_addi(s, t0, rz, 0);
  rtl_sub(s, dest, t0, src1); 
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  rtl_li(s, t0, width);
  rtl_li(s, t1, 8);
  rtl_mulu_lo(s, t0, t0, t1);
  rtl_subi(s, t0, t0, 31);
  rtl_neg(s, t0, t0);
  rtl_li(s, t1, -0x80000000);
  rtl_sra(s, t0, t1, t0);
  rtl_or(s, dest, t0, src1);
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  rtl_li(s, t0, width);
  rtl_li(s, t1, 8);
  rtl_mulu_lo(s, t0, t0, t1);
  rtl_subi(s, t0, t0, 31);
  rtl_neg(s, t0, t0);

  rtl_li(s, t1, -0x80000000);
  rtl_sra(s, t0, t1, t0);
  rtl_not(s, t0, t0);
  rtl_and(s, dest, t0, src1);
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  rtl_li(s, t0, width);
  rtl_li(s, t1, 8);
  rtl_mulu_lo(s, t0, t0, t1);
  rtl_subi(s, t0, t0, 1);
  rtl_srl(s, dest, src1, t0);
}
#endif
