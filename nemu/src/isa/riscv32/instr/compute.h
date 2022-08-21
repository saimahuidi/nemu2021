
#include "cpu/decode.h"
#include "cpu/exec.h"
#include "debug.h"
#include "rtl/rtl.h"
def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm);
  rtl_addi(s, ddest, ddest, s->pc);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti) {
  rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->simm);
}

def_EHelper(sltu) {
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
  rtl_andi(s, s0, &id_src2->imm, 0b11111);
  rtl_sra(s, ddest, dsrc1, s0);
}

def_EHelper(srli) {
  rtl_andi(s, s0, &id_src2->imm, 0b11111);
  rtl_srl(s, ddest, dsrc1, s0);
}
def_EHelper(slli) {
  rtl_andi(s, s0, &id_src2->imm, 0b11111);
  rtl_sll(s, ddest, dsrc1, s0);
}
def_EHelper(sll) {
  rtl_andi(s, s0, dsrc2, 0b11111);
  rtl_sll(s, ddest, dsrc1, s0);
}

def_EHelper(srl) {
  rtl_andi(s, s0, dsrc2, 0b11111);
  rtl_srl(s, ddest, dsrc1, s0);
}

def_EHelper(sra) {
  rtl_andi(s, s0, dsrc2, 0b11111);
  rtl_sra(s, ddest, dsrc1, s0);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->simm);
}
def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhsu) {
  TODO();
}

def_EHelper(mulhu) {
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu) {
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}