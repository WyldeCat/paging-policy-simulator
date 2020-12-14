/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "pin.H"

int sock;
struct timeval start;

long get_timestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec - start.tv_sec) * 1000000L +
        (now.tv_usec - start.tv_usec);
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
    long type = 0;
    long timestamp = get_timestamp();

    write(sock, &type, sizeof(type));
    write(sock, &ip, sizeof(ip));
    write(sock, &addr, sizeof(addr));
    write(sock, &timestamp, sizeof(timestamp));
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    long type = 1;
    long timestamp = get_timestamp();

    write(sock, &type, sizeof(type));
    write(sock, &ip, sizeof(ip));
    write(sock, &addr, sizeof(addr));
    write(sock, &timestamp, sizeof(timestamp));
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
   close(sock); 
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

    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "./socket");
    int ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("connect");
        return -1;
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    ret = gettimeofday(&start, NULL);
    if (ret < 0) {
        perror("gettimeofday");
        return -1;
    }

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
