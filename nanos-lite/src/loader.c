#include <proc.h>
#include <elf.h>
#include <ramdisk.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV  // see /usr/include/elf.h to get the right type
#else
# error Unsupported ISA
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  // load the elfheader
  Elf32_Ehdr header; 
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &header, sizeof(Elf32_Ehdr));
  assert(*(uint32_t *)header.e_ident == 0x464c457f);
  assert(header.e_machine == EXPECT_TYPE);
  // load the information of segment header
  int seg_num = header.e_phnum;
  Elf32_Phdr pheader;
  int phdr_offset = header.e_phoff;
  for (int i = 0; i < seg_num; i++) {
    // load the needed segment
    fs_lseek(fd, phdr_offset + i * sizeof(Elf32_Phdr), SEEK_SET);
    fs_read(fd, &pheader, sizeof(Elf32_Phdr));
    if (pheader.p_type == PT_LOAD) {
      uint32_t offset = pheader.p_offset;
      uint8_t *vaddr = (void *)pheader.p_vaddr;
      uint32_t fsize = pheader.p_filesz;
      uint32_t msize = pheader.p_memsz;
      fs_lseek(fd, offset, SEEK_SET);
      fs_read(fd, vaddr, fsize);
      memset(vaddr + fsize, 0, msize - fsize);
    }
  }
  return header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

