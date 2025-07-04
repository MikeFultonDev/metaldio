
#include <stdio.h>
#include <stdlib.h>

/*
 * Simple example showing how to disassemble an instruction using ASMADOP:
 * https://tech.mikefulton.ca/disasm
 * The example below disassembles an MVC instruction
 */
#pragma pack(1)
struct fncode {
  int left_justify:1;
  int full_disassembly:1;
  int fill_component_fields:1;
  int fill_instruction_characteristics:1;
  int unk:3;
  int displacement_in_hex:1;

  int uni:1;
  int dos:1;
  int s370:1;
  int xa:1;
  int esa:1;
  int zs1:1;
  int zs2:1;
  int zs3:1;
  int zs4:1;
  int zs5:1;
  int zs6:1;
  int zs7:1;
  int zs8:1;
  int zs9:1;
  int zsa:1;
  int pad:1;

  int version:8;
};

struct mnemonic {
  char data[8];
};
struct disinst {
  unsigned short len;
  char text[40];
};
struct optable {
  char data[92];
};
struct instchar {
  char data[8];
};
struct workarea {
  char data[256];
};

struct disasm {
  char* bininst;
  struct fncode*  fn_code;
  struct mnemonic* mnem;
  struct disinst* dis_inst;
  struct optable* op_tbl;
  struct instchar* inst_char;
  struct workarea* work_area;
};
#pragma pack(pop)

typedef int (ASMADOP_FP)(struct disasm disasm);

int main(int argc, char* argv[])
{
  char bin_inst[] = "\xD2\x1B\xC0\x00\x10\x00";
  struct fncode fn_code = { 0 };
  struct mnemonic mnem = { 0 }; 
  struct disinst dis_inst = { 0 };
  struct optable op_tbl = { 0 };
  struct instchar inst_char = { 0 };
  struct workarea work_area = { 0 };
  int rc;

  fn_code.left_justify = 1;
  fn_code.full_disassembly = 1;
  fn_code.fill_component_fields = 1;
  fn_code.fill_instruction_characteristics = 1;
  fn_code.version = 4;
  
  struct disasm disasm = { bin_inst, &fn_code, &mnem, &dis_inst, &op_tbl, &inst_char, &work_area };
  ASMADOP_FP* fp = (ASMADOP_FP*) fetch("ASMADOP");
  if (!fp) {
    fprintf(stderr, "Unable to fetch ASMADOP\n");
  }
  rc = fp(disasm);
  printf("Return code from disassembly: %d\n", rc);
  if (!rc) {
    printf("Disassembly: %*s\n", disasm.dis_inst->len, disasm.dis_inst->text);
  }
}