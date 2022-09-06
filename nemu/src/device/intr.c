#include <isa.h>
#include <stdbool.h>

void dev_raise_intr() {
    cpu.INTR = true;
}
