#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <map>
#include <vector>
#include <algorithm>
#include "branch_pred.h"
#include "libdft_api.h"
#include "libdft_core.h"
#include "syscall_desc.h"
#include "pin.H"

#define BYTE 1
#define WORD 2
#define LONG_WORD 4

extern REG thread_ctx_ptr;
extern syscall_desc_t syscall_desc[SYSCALL_MAX];
extern ins_desc_t ins_desc[XED_ICLASS_LAST];

const char *tracefile = "vmtrace.txt";
size_t tainted_data_size;
FILE *fp;

std::map<ADDRINT, std::string> opcodes;

static void log_ins_addr(ADDRINT addr)
{
    fprintf(fp, "0x%x: %s\n", addr, opcodes[addr].c_str());
}

static ADDRINT PIN_FAST_ANALYSIS_CALL
assert_reg32(thread_ctx_t *thread_ctx, uint32_t reg)
{
	return thread_ctx->vcpu.gpr[reg];
}

static ADDRINT PIN_FAST_ANALYSIS_CALL
assert_reg16(thread_ctx_t *thread_ctx, uint32_t reg)
{
	return thread_ctx->vcpu.gpr[reg];
}

static ADDRINT PIN_FAST_ANALYSIS_CALL
assert_mem(ADDRINT addr, uint32_t size)
{
    //printf("DEBUG 'size': %d\n", size);
    if (size == BYTE)
        return tagmap_getb(addr);
    if (size == WORD)
        return tagmap_getw(addr);
    if (size == LONG_WORD)
        return tagmap_getl(addr);
    
    printf("DEBUG 'size': %d\n", size);
    return 0;
}


static ADDRINT PIN_FAST_ANALYSIS_CALL
always_false()
{
    return 0;
}
/*
static void dta_instrument_lea_ins(INS ins)
{
    opcodes[INS_Address(ins)] = INS_Disassemble(ins);

    INS_InsertIfCall(ins,
                     IPOINT_BEFORE,
                     (AFUNPTR)assert_mem_explicit,
                     IARG_FAST_ANALYSIS_CALL,
                     IARG_MEMORYOP_PTR,
                     IARG_END);

    INS_InsertThenCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR)log_ins_addr,
                       IARG_INST_PTR,
                       IARG_END);
}
*/
static void dta_instrument_mov_ins(INS ins)
{
    opcodes[INS_Address(ins)] = INS_Disassemble(ins);

    if (INS_OperandIsReg(ins, 1))
    {
        REG reg = INS_OperandReg(ins, 1);

        if (REG_is_gr32(reg))
        {

            INS_InsertIfCall(ins,
                             IPOINT_BEFORE,
                             (AFUNPTR)assert_reg32,
                             IARG_FAST_ANALYSIS_CALL,
                             IARG_REG_VALUE, thread_ctx_ptr,
                             IARG_UINT32, REG32_INDX(reg),
                             IARG_END);
        }
        else
        {
            INS_InsertIfCall(ins,
                             IPOINT_BEFORE,
                             (AFUNPTR)assert_reg16,
                             IARG_FAST_ANALYSIS_CALL,
                             IARG_REG_VALUE, thread_ctx_ptr,
                             IARG_UINT32, REG16_INDX(reg),
                             IARG_END);
        }
    }
    /*
    * Instrument Memory read.
     */
    else if (INS_OperandIsMemory(ins , 1) && INS_IsMemoryRead(ins))
    {
        INS_InsertIfCall(ins,
                         IPOINT_BEFORE,
                         (AFUNPTR)assert_mem,
                         IARG_FAST_ANALYSIS_CALL,
                         IARG_MEMORYREAD_EA,
                         IARG_MEMORYREAD_SIZE,
                         IARG_END);
    }
    else if (INS_OperandIsMemory(ins , 0) && INS_IsMemoryWrite(ins))
    {
        INS_InsertIfCall(ins,
                         IPOINT_BEFORE,
                         (AFUNPTR)assert_mem,
                         IARG_FAST_ANALYSIS_CALL,
                         IARG_MEMORYWRITE_EA,
                         IARG_MEMORYWRITE_SIZE,
                         IARG_END);
    }
    else
    {
        INS_InsertIfCall(ins,
                         IPOINT_BEFORE,
                         (AFUNPTR)always_false,
                         IARG_FAST_ANALYSIS_CALL,
                         IARG_END);
    }
    
    INS_InsertThenCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR)log_ins_addr,
                       IARG_INST_PTR,
                       IARG_END);
}

static void post_read_hook(syscall_ctx_t *ctx)
{
    if (unlikely((long)ctx->ret <= 0))
        return;
    
    if (ctx->arg[SYSCALL_ARG0] == STDIN_FILENO) {
        tagmap_setn((uintptr_t)ctx->arg[SYSCALL_ARG1], (size_t)ctx->ret, 0x01);
	fprintf(fp, "INPUT TAINTED, addr: 0x%x\n", ctx->arg[SYSCALL_ARG1]);    
    }	
}


static void fini(INT32 code, void* v)
{
    fprintf(fp, "---------- trace complete --------------\n");
    fclose(fp);
}

int main(int argc, char **argv)
{
    fp = fopen(tracefile, "w");

	//PIN_InitSymbols();
       
	if (unlikely(PIN_Init(argc, argv)))
		goto err;

	if (unlikely(libdft_init() != 0))
		goto err;
	
    (void)ins_set_post(&ins_desc[XED_ICLASS_MOV],
			dta_instrument_mov_ins);
 
  //  (void)ins_set_post(&ins_desc[XED_ICLASS_LEA],
    //                    dta_instrument_lea_ins);

    syscall_set_post(&syscall_desc[__NR_read], post_read_hook);

    //INS_AddInstrumentFunction(instrument_ins, NULL);
    PIN_AddFiniFunction(fini, NULL);

    PIN_StartProgram();

    return EXIT_SUCCESS;

err:	/* error handling */

	return EXIT_FAILURE;
}
