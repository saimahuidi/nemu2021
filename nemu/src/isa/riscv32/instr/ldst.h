#include "cpu/decode.h"
#include "cpu/exec.h"
#include "debug.h"
#include "rtl/rtl.h"

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