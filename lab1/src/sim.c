/* Pranav Subramaniam, psubramaniam*/
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

//Constants
#define ADD 1
#define B 2
#define BLE 3
#define LDURH 4
#define STURB 5
#define SUBS 6
#define BL 7
#define ADDI 8
#define BEQ 9
#define CBNZ 10
#define LSL 11
#define STURH 12
#define MUL 13
#define BR 14
#define ADDIS 15
#define BNE 16
#define CBZ 17
#define LSR 18
#define STURW 19
#define SDIV 20
#define ADDS 21
#define BGT 22
#define EOR 23
#define MOVZ 24
#define SUB 25
#define UDIV 26
#define AND 27
#define BLT 28
#define LDUR 29
#define ORR 30
#define SUBI 31
#define HLT 32
#define ANDS 33
#define BGE 34
#define LDURB 35
#define STUR 36
#define SUBIS 37
#define CMP 38
#define BCOND 39

//Format Types
#define FR 1
#define FB 2
#define FCB 3
#define FD 4
#define FI 5
#define FIW 6

//Global Variables
uint32_t cur_instr;  //Current instruction
int format; //Current format
int instr_name; //store the name of the instruction here

//B format variables
int64_t baddr; //branch address

//CB format variables
int64_t cbaddr; //conditional branch address
//also uses rt

//R format variables
uint32_t rm; //second register source operand
uint32_t shamt; //optional shift amount
//also uses rn and rd

//D format variables
uint32_t rt; //can either be source or destination, depending on load or store
uint32_t address; //address to store to or load from
//also uses rn

//I format variables
uint32_t imm; //the immediate value
//also uses rn and rd

//IW format variables
int64_t wimm; //the immediate value
uint32_t hw; //the (optional) shift value
/*Explanation of how hw works
hw can be b00, b01, b10, or b11 and can shift wimm left by 0,16,32, or 48
so we shift wimm by hw*16
*/
//also uses rd

//variables common between formats
uint32_t rd; //register to be stored to
uint32_t rn; //first register source operand
//rt should be here too, but whatever...

void fetch()
{
  cur_instr = mem_read_32(CURRENT_STATE.PC);
}

/************************************DECODE FUNCTIONS****************************************/

void getinfoFB(uint32_t instr)
{
   baddr = instr & 0x03ffffff;
   //next, we need to sign-extend this...
   baddr = baddr << 38;
   baddr = baddr >> 38;
   baddr = baddr << 2; //to ensure 32-bit alignment
    
}

void getinfoFCB(uint32_t instr)
{
  cbaddr = instr & 0x00ffffe0;
  cbaddr = cbaddr >> 5;
  //now to sign-extend...
  cbaddr = cbaddr << 45;
  cbaddr = cbaddr >> 45;
  cbaddr = cbaddr << 2; //to ensure 32-bit alignment
  rt = instr & 0x0000001f; 
}

void getinfoFR(uint32_t instr)
{
  rd = instr & 0x0000001f; 
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  shamt = instr & 0x0000fc00;
  shamt = shamt >> 10;
  rm = instr & 0x001f0000;
  rm = rm >> 16;
  
}

void getinfoFD(uint32_t instr)
{
  rt = instr & 0x0000001f;
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  address = instr & 0x001ff000; 
  address = address >> 12;
}

void getinfoFI(uint32_t instr)
{
  rd = instr & 0x0000001f;
  rn = instr & 0x000003e0;
  rn = rn >> 5;
  imm = instr & 0x003ffc00;
  imm = imm >> 10;
}

void getinfoFIW(uint32_t instr)
{
  rd = instr & 0x0000001f;
  wimm = instr & 0x001fffe0;
  wimm = wimm >> 5;
  hw = instr & 0x00600000;
  hw = hw >> 21;
}

//set relevant variables based on opcode 
void set_format_ops(uint32_t instr)
{
  uint32_t opc = instr >> 21; //get the top 11 bits of the instruction
  if (opc >= 160 && opc <= 191){
     format = FB; 
     instr_name = B;
     getinfoFB(instr);
  }else if (opc == 448){
     format = FD;
     instr_name = STURB;
     getinfoFD(instr);
  }else if (opc == 450){
     format = FD;
     instr_name = LDURB;
     getinfoFD(instr);
  }else if (opc >= 672 && opc <= 679){
     format = FCB;
     instr_name = BCOND;
     getinfoFCB(instr);
  }else if (opc == 1112){
     format = FR;
     instr_name = ADD;
     getinfoFR(instr);
  }else if (opc == 1368){
     format = FR;
     instr_name = ADDS;
     getinfoFR(instr);
  }else if (opc >= 1160 && opc <= 1161){
     format = FI;
     instr_name = ADDI;
     getinfoFI(instr);
  }else if (opc >= 1416 && opc <= 1417){
     format = FI;
     instr_name = ADDIS;
     getinfoFI(instr);
  }else if (opc == 1104){
     format = FR;
     instr_name = AND;
     getinfoFR(instr);
  }else if (opc == 1872){
     format = FR;
     instr_name = ANDS;
     getinfoFR(instr);
  }else if (opc >= 1440 && opc <= 1447){
     format = FCB;
     instr_name = CBZ;
     getinfoFCB(instr);
  }else if (opc >= 1448 && opc <= 1455){
     format = FCB;
     instr_name = CBNZ;
     getinfoFCB(instr);
  }else if (opc == 1616){
     format = FR;
     instr_name = EOR;
     getinfoFR(instr);
  }else if (opc == 1986){
     format = FD;
     instr_name = LDUR;
     getinfoFD(instr);
  }else if (opc == 962){
     format = FD;
     instr_name = LDURH;
     getinfoFD(instr);
  }else if (opc == 1691){
     format = FR;
     instr_name = LSL;
     getinfoFR(instr);
  }else if (opc == 1690){
     format = FR;
     instr_name = LSR;
     getinfoFR(instr);
  }else if (opc >= 1684 && opc <= 1687){
     format = FIW;
     instr_name = MOVZ;
     getinfoFIW(instr);
  }else if (opc == 1360){
     format = FR;
     instr_name = ORR;
     getinfoFR(instr);
  }else if (opc == 1984){
     format = FD;
     instr_name = STUR;
     getinfoFD(instr);
  }else if (opc == 960){
     format = FD;
     instr_name = STURH;
     getinfoFD(instr);
  }else if (opc == 1472){
     format = FD;
     instr_name = STURW;
     getinfoFD(instr);
  }else if (opc == 1624){
     format = FR;
     instr_name = SUB;
     getinfoFR(instr);
  }else if (opc >= 1672 && opc <= 1673){
     format = FI;
     instr_name = SUBI;
     getinfoFI(instr);
  }else if (opc == 1880){
     format = FR;
     instr_name = SUBS;
     getinfoFR(instr);
  }else if (opc >= 1928 && opc <= 1929){
     format = FI;
     instr_name = SUBIS;
     getinfoFI(instr);
  }else if (opc == 1240){
     format = FR;
     instr_name = MUL;
     getinfoFR(instr);
  }else if (opc == 1238){
     format = FR;
     if (shamt != 2){
        instr_name = UDIV;
     }else{
        instr_name = SDIV;
     } //could be UDIV also, this is decided later by shamt
     getinfoFR(instr);
  }else if (opc >= 1184 && opc <= 1215){
     format = FB;
     instr_name = BL;
     getinfoFB(instr);
  }else if (opc == 1712){
     format = FR;
     instr_name = BR;
     getinfoFR(instr);
  }else if (opc == 1698){
     format = FIW;
     instr_name = HLT;
     getinfoFIW(instr);
  }else {
     fprintf(stderr, "Error: Instruction not covered, %d\n", opc);
     exit(1);
  }
}


void decode()
{
  set_format_ops(cur_instr); 
}

//execute ALU ops
void exec_ALU(int64_t op1, int64_t op2, int *N, int *Z, int *V, int *C, int64_t *result, int name)
{
  if (name == ADD){
     *result = op1 + op2;
  }else if (name == ADDI){
     *result = op1 + op2;
  }else if (name == ADDIS){
     *result = op1 + op2;
     /*if ((*result < 0) && (op1 > 0) && (op2 > 0)){
        *V = 1;
     }else if ((*result > 0) && (op1 < 0) && (op2 < 0)){
        *V = 1;
     }else {
        *V = 0;
     }
     if (*V == 1){
        *C = 1;
     } Realized later that this part is not needed*/
    // }else if ...figure this out later 
  }else if (name == ADDS){
     *result = op1 + op2;
     /*if ((*result < 0) && (op1 > 0) && (op2 > 0)){
        *V = 1;
     }else if ((*result > 0) && (op1 < 0) && (op2 < 0)){
        *V = 1;
     }else {
        *V = 0;
     }*/
  }else if (name == SUB){
     *result = op1 - op2;
  }else if (name == SUBI){
     *result = op1 - op2;
  }else if (name == SUBIS){
     *result = op1 - op2;
     /*if ((*result < 0) && (op1 > 0) && (op2 < 0)){
        *V = 1;
     }else if ((*result > 0) && (op1 < 0) && (op2 > 0)){
        *V = 1;
     }else {
        *V = 0;
     }*/
  }else if (name == SUBS){
     *result = op1 - op2;
     /*if ((*result < 0) && (op1 > 0) && (op2 < 0)){
        *V = 1;
     }else if ((*result > 0) && (op1 < 0) && (op2 > 0)){
        *V = 1;
     }else {
        *V = 0;
     }*/
  }else if (name == AND){
     *result = op1 & op2;
  }else if (name == ANDS){
     *result = op1 & op2;
  }else if (name == EOR){
     *result = op1^op2;
  }else if (name == ORR){
     *result = op1 | op2;
  }else if (name == MUL){
     *result = op1 * op2;
    /* if ((op1 != 0) && (op2 != 0) && (*result/op1 != op2)){
        *V = 1;
     }else {
        *V = 0;
     }*/
  } 
  if (*result < 0){
     *N = 1;
  }else{
     *N = 0;
  }
  if (*result == 0){
     *Z = 1;
  }else{
     *Z = 0;
  }
      
}

void exec_ADD()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
 
}

void exec_ADDI()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = imm;
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
 
}

void exec_ADDIS()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = imm;
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  NEXT_STATE.FLAG_N = N;
  NEXT_STATE.FLAG_Z = Z;
}

void exec_ADDS()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  NEXT_STATE.FLAG_N = N;
  NEXT_STATE.FLAG_Z = Z;
 
}

void exec_SUB()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
 
}

void exec_SUBI()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = imm;
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
 
}

void exec_SUBIS()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = imm;
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  NEXT_STATE.FLAG_N = N;
  NEXT_STATE.FLAG_Z = Z;
  
}

void exec_SUBS()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  NEXT_STATE.FLAG_N = N;
  NEXT_STATE.FLAG_Z = Z;
 
}

void exec_AND()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  
}

void exec_ANDS()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  NEXT_STATE.FLAG_N = N;
  NEXT_STATE.FLAG_Z = Z;
 
}

void exec_MUL()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  
}

void exec_EOR()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  
}

void exec_ORR()
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  int64_t result;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  exec_ALU(op1, op2, &N, &Z, &V, &C, &result, instr_name);
  NEXT_STATE.REGS[rd] = result; 
  
}

void exec_SDIV() //create a case that takes care of UDIV here, too
{
  int64_t op1;
  int64_t op2;
  int N;
  int Z;
  int V;
  int C;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  NEXT_STATE.REGS[rd] = op1/op2; 
  
}

void exec_UDIV() //create a case that takes care of UDIV here, too
{
  uint64_t op1;
  uint64_t op2;
  int N;
  int Z;
  int V;
  int C;
  
  op1 = CURRENT_STATE.REGS[rn];
  op2 = CURRENT_STATE.REGS[rm];
  NEXT_STATE.REGS[rd] = op1/op2; 
  
}

void exec_LSL()
{
  shamt = cur_instr & 0x0000fc00;
  shamt = shamt >> 10;
  shamt = 63 - shamt;
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] << shamt;
}

void exec_LSR()
{
  shamt = cur_instr & 0x003f0000;
  shamt = shamt >> 16;
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] >> shamt;
}

void exec_B()
{
  NEXT_STATE.PC = CURRENT_STATE.PC + baddr; 
}

void exec_BCOND()
{
  int z = CURRENT_STATE.FLAG_Z;
  int n = CURRENT_STATE.FLAG_N;
  
  
  if (rt == 0){ //BEQ
     if (z == 1){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else if (rt == 1){ //BNE
     if (z == 0){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else if (rt == 12){ //BGT
     if ((z == 0) && (n == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else if (rt == 11){ //BLT
     if (n != 0){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else if (rt == 13){ //BLE
     if ((z != 0) || (n != 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else if (rt == 10){ //BGE
     if (n == 0){
        NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
     }else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
     }
  }else {
     fprintf(stderr, "ERROR: Not a BCOND instruction\n");
     exit(1);
  }
}

void exec_CBZ()
{
  if (CURRENT_STATE.REGS[rt] == 0){
     NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
  }else {
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
  }
}

void exec_CBNZ()
{
  if (CURRENT_STATE.REGS[rt] != 0){
     NEXT_STATE.PC = CURRENT_STATE.PC + cbaddr;
  }else {
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
  }
}

void exec_BL()
{
  NEXT_STATE.REGS[30] = CURRENT_STATE.PC + 4;
  NEXT_STATE.PC = CURRENT_STATE.PC + baddr;
}

void exec_BR()
{
  uint64_t target = CURRENT_STATE.REGS[rn];
  NEXT_STATE.PC = target;
}

void exec_LDUR()
{
  int64_t tmp1;
  int64_t tmp2;
  int64_t value;
  uint64_t base_address = CURRENT_STATE.REGS[rn] + address;
  
  tmp1 = mem_read_32(base_address);
  tmp2 = mem_read_32(base_address + 4);
  value = (tmp2 << 32) | tmp1;
  NEXT_STATE.REGS[rt] = value;
}

void exec_LDURB()
{
  int64_t tmp1;
  int64_t msk = 0xff;
  uint64_t base = CURRENT_STATE.REGS[rn] + address;
  tmp1 = mem_read_32(base);
  uint64_t value = tmp1 & msk;
  NEXT_STATE.REGS[rt] = value;
}

void exec_LDURH()
{
  int64_t tmp1;
  int64_t msk = 0xffff;
  uint64_t base = CURRENT_STATE.REGS[rn] + address;
  tmp1 = mem_read_32(base);
  uint64_t value = tmp1 & msk;
  NEXT_STATE.REGS[rt] = value;
}

void exec_STUR()
{
  int64_t tmp1;
  int64_t tmp2;
  int64_t value;
  uint64_t base_address = CURRENT_STATE.REGS[rn] + address;
  
  tmp1 = CURRENT_STATE.REGS[rt];
  tmp2 = CURRENT_STATE.REGS[rt] >> 32;

  mem_write_32(base_address, tmp1);
  mem_write_32(base_address + 4, tmp2);
}

void exec_STURB()
{
  int32_t firstbyte = CURRENT_STATE.REGS[rt] & 0xff;
  uint64_t base_address = CURRENT_STATE.REGS[rn] + address;
  uint32_t tmp1 = ~0xff;
  uint32_t tmp2 = mem_read_32(base_address);
  uint32_t value = tmp1 & tmp2;
  uint32_t result = value | firstbyte;
  mem_write_32(base_address, result); 
}

void exec_STURH()
{
  int32_t t2bytes = CURRENT_STATE.REGS[rt] & 0xffff;
  uint64_t base_address = CURRENT_STATE.REGS[rn] + address;
  uint32_t tmp1 = ~0xffff;
  uint32_t tmp2 = mem_read_32(base_address);
  uint32_t value = tmp1 & tmp2;
  uint32_t result = value | t2bytes;
  mem_write_32(base_address, result); 
}

void exec_MOVZ()
{
  NEXT_STATE.REGS[rd] = wimm << (hw*16); 
}

void exec_HLT()
{
 RUN_BIT = 0;
}

void execute()
{
  switch(instr_name){
     case ADD:
       exec_ADD();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case ADDI:
       exec_ADDI();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case ADDIS:
       exec_ADDIS();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case ADDS:
       exec_ADDS();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case AND:
       exec_AND();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case ANDS:
       exec_ANDS();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case B:
       exec_B();
       break;
     case BCOND:
       exec_BCOND();
       break;
     case CBNZ:
       exec_CBNZ();
       break;
     case CBZ:
       exec_CBZ();
       break;
     case EOR:
       exec_EOR();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case LDUR:
       exec_LDUR();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case LDURB:
       exec_LDURB();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case LDURH:
       exec_LDURH();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case LSL:
       exec_LSL();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case LSR:
       exec_LSR();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case MOVZ:
       exec_MOVZ();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case ORR:
       exec_ORR();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case STUR:
       exec_STUR();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case STURB:
       exec_STURB();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case STURH:
       exec_STURH();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case SUB:
       exec_SUB();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case SUBI:
       exec_SUBI();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case SUBIS:
       exec_SUBIS();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case SUBS:
       exec_SUBS();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case MUL:
       exec_MUL();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case SDIV:
       exec_SDIV();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case UDIV:
       exec_UDIV();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     case HLT:
       exec_HLT();
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       break;
     /*case CMP:
       exec_CMP(); 
       break; might not be needed*/
     case BL:
       exec_BL(); 
       break;
     case BR:
       exec_BR(); 
       break;
     default:
       fprintf(stderr, "Not an instruction\n");
       exit(1);
  }
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    fetch();
    decode();
    execute();

}
