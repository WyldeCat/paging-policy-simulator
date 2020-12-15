/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "pin.H"
#include "page_policy.hpp"
#include "simulate.hpp"

struct timeval start;
PIN_MUTEX sim_lock;

long get_timestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec - start.tv_sec) * 1000000L +
        (now.tv_usec - start.tv_usec);
}

VOID send_record(long type, long ip, long addr, long timestamp)
{
    char tmp[8 * 4];
    *(long *)(tmp + 0) = type;
    *(long *)(tmp + 8) = ip;
    *(long *)(tmp + 16) = addr;
    *(long *)(tmp + 24) = timestamp;

    while (!PIN_MutexTryLock(&sim_lock));
    add_memtrace(type, ip, addr, timestamp);
    PIN_MutexUnlock(&sim_lock);
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
    long timestamp = get_timestamp();
    send_record(0, (long)ip, (long)addr, timestamp);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    long timestamp = get_timestamp();
    send_record(1, (long)ip, (long)addr, timestamp);
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
    if (PIN_Init(argc, argv)) return Usage();

    PIN_MutexInit(&sim_lock);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_THREAD_UID uid;
    THREADID tid = PIN_SpawnInternalThread(simulate_loop, nullptr, 0, &uid);
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
