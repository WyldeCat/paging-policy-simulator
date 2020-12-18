/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "pin.H"
#include "page_policy.hpp"
#include "simulate.hpp"

PIN_MUTEX sim_lock;
PIN_THREAD_UID uid;
simulate_args args;
volatile extern bool is_initialized;
size_t curr = 0;

static struct timeval start;
static long get_timestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec - start.tv_sec) * 1000000L +
        (now.tv_usec - start.tv_usec);
}

VOID send_record(int type, long ip, long addr)
{
    Record r{type, ip, addr, 0};
    while (!PIN_MutexTryLock(&sim_lock));
    if (start.tv_usec == 0 && start.tv_sec == 0) {
        r.time_stamp = 0;
    }
    else {
        r.time_stamp = curr + get_timestamp();
        curr = r.time_stamp;
    }
    add_memtrace(r);
    gettimeofday(&start, NULL);
    PIN_MutexUnlock(&sim_lock);
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
    send_record(0, (long)ip, (long)addr);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    send_record(1, (long)ip, (long)addr);
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    send_record(2, 0, 0);

    fprintf(stderr, "[Simulator][INFO] Application Finished!\n");
    fprintf(stderr, "[Simulator][INFO] Waiting for simulator to finish...\n");

    BOOL ret;
    INT32 exitcode;

    ret = PIN_WaitForThreadTermination(uid, PIN_INFINITE_TIMEOUT, &exitcode);
    assert(ret);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    // HACK
    args.mem = argv[5];
    args.policy = argv[6];
    args.num_buffer = argv[7];
    args.size_buffer = argv[8];
    args.csv_out = argv[9];
    args.num_interval = argv[10];
    for (int i = 5; i < argc - 5; i++) {
        argv[i] = argv[i + 6];
    }
    argc -= 6;

    if (PIN_Init(argc, argv)) return Usage();

    PIN_MutexInit(&sim_lock);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    THREADID tid = PIN_SpawnInternalThread(simulate_loop, &args, 0, &uid);
    assert(tid != INVALID_THREADID);

    int ret = gettimeofday(&start, NULL);
    if (ret < 0) {
        perror("gettimeofday");
        return -1;
    }

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
