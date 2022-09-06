#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
             f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((32 + 4 + 1) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  (32 * XLEN)
#define OFFSET_STATUS (33 * XLEN)
#define OFFSET_EPC    (34 * XLEN)
#define OFFSET_SCRATCH    (35 * XLEN)

.align 3
.globl __am_asm_trap
__am_asm_trap:
  csrrw sp, mscratch, sp   // (1) atomically exchange sp and mscratch
  bnez  sp, save_context    // (2) take the branch if we trapped from user
  csrrw sp, mscratch, sp   // (3) exchange back

save_context: 
  addi sp, sp, -CONTEXT_SIZE

  sw    x1, (4)(sp) 
  csrr  x1, mscratch
  sw    x1, (8)(sp)
  csrrwi x0, mscratch, 0

  MAP(REGS, PUSH)

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc
  csrr t3, mscratch

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
  STORE t3, OFFSET_SCRATCH(sp)

  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1

  mv a0, sp
  jal __am_irq_handle
  
  mv sp, a0

  

  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  LOAD t3, OFFSET_SCRATCH(sp)
  csrw mstatus, t1
  csrw mepc, t2
  csrw mscratch, t3

  MAP(REGS, POP)
  lw x1, (4)(sp)

  addi sp, sp, CONTEXT_SIZE

  csrrw x1, mscratch, x1   // (1) atomically exchange sp and mscratch
  lw    x1, (-CONTEXT_SIZE + 8)(sp)
  beqz  x1, kernel

  sw    x1, (-CONTEXT_SIZE + 8)(sp)
  add   x1, x0, sp
  lw    sp, (-CONTEXT_SIZE + 8)(sp)
kernel:
  csrrw x1, mscratch, x1   // (1) atomically exchange sp and mscratch
  mret
  