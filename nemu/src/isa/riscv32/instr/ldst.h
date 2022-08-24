#include "cpu/decode.h"
#include "cpu/exec.h"
#include "debug.h"
#include "isa.h"
#include "rtl-basic.h"
#include "rtl/rtl.h"
#include <stdio.h>

def_EHelper(lb) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
}
def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}
def_EHelper(lh) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 2);
}
def_EHelper(lhu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}
def_EHelper(lw) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(sw) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(sh) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sb) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(jal) {
  rtl_li(s, ddest, s->snpc);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->simm);
}
def_EHelper(jalr) {
  rtl_addi(s, s1, dsrc1, id_src2->simm);
  rtl_andi(s, &s->dnpc, s1, -2);
  rtl_li(s, ddest, s->snpc);
}

def_EHelper(beq) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_EQ, ddest, dsrc1, *s0);
}
def_EHelper(bne) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_NE, ddest, dsrc1, *s0);
}
def_EHelper(blt) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_LT, ddest, dsrc1, *s0);
}
def_EHelper(bge) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_GE, ddest, dsrc1, *s0);
}
def_EHelper(bltu) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_LTU, ddest, dsrc1,*s0);
}
def_EHelper(bgeu) {
  rtl_addi(s, s0, &s->pc, id_src2->simm);
  rtl_jrelop(s, RELOP_GEU, ddest, dsrc1, *s0);
}

def_EHelper(ecall) {
  *s0 = isa_raise_intr(8, s->pc);
  rtl_jr(s, s0);
}

def_EHelper(mret) {
  rtl_j(s, cpu.mepc + 4);
}

def_EHelper(csrrw) {
  switch (id_src2->imm) {
    case 0x305:
      *s0 = cpu.mtvec;
      cpu.mtvec = *dsrc1;
      *ddest = *s0;
      break;
    case 0x300:
      *s0 = cpu.mstatus;
      cpu.mstatus = *dsrc1;
      *ddest = *s0;
      break;
    case 0x341:
      *s0 = cpu.mepc;
      cpu.mepc = *dsrc1;
      *ddest = *s0;
      break;
    case 0x342:
      *s0 = cpu.mcause;
      cpu.mcause = *dsrc1;
      *ddest = *s0;
      break;
    default:
      panic("no implement\n");
  }
}

def_EHelper(csrrs) {
  switch (id_src2->imm) {
    case 0x300:
      *s0 = cpu.mstatus;
      cpu.mstatus = *dsrc1;
      *ddest = *s0;
      break;
    case 0x341:
      *s0 = cpu.mepc;
      cpu.mepc = *dsrc1;
      *ddest = *s0;
      break;
    case 0x342:
      *s0 = cpu.mcause;
      cpu.mcause = *dsrc1;
      *ddest = *s0;
      break;
  }
}

def_EHelper(csrrwi) {

}

def_EHelper(csrrsi) {

}

def_EHelper(csrrci) {

}