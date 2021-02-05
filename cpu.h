#pragma once
#include "types.h"
class Mem;

class CPU {
public:

    CPU(Mem* );
    ~CPU();
    Mem* mem;
    Byte A, X, Y;
    Byte SP;
    Word PC;

    /*  The carry flag is set if the last operation caused an 
        overflow from bit 7 of the result or an underflow from
        bit 0. This condition is set during arithmetic, 
        comparison and during logical shifts. It can be 
        explicitly set using the 'Set Carry Flag' (SEC) 
        instruction and cleared with 'Clear Carry Flag' (CLC). 
    */
    Byte Carry   : 1;

    /* The zero flag is set if the result of the last operation as was zero. */
    Byte Zero   : 1;

    /*  The interrupt disable flag is set if the program has executed a 
        'Set Interrupt Disable' (SEI) instruction. While this flag is set the 
        processor will not respond to interrupts from devices until it is 
        cleared by a 'Clear Interrupt Disable' (CLI) instruction.
    */
    Byte InterruptDisable  : 1;
    
    /*  While the decimal mode flag is set the processor will obey the rules
        of Binary Coded Decimal (BCD) arithmetic during addition and subtraction.
        The flag can be explicitly set using 'Set Decimal Flag' (SED) and cleared
        with 'Clear Decimal Flag' (CLD). 
    */
    Byte DecimalMode : 1;
    
    /*  The break command bit is set when a BRK instruction has been executed and
        an interrupt has been generated to process it. */
    Byte BreakCommand : 1;

    /*  The overflow flag is set during arithmetic operations if the result has 
        yielded an invalid 2's complement result (e.g. adding to positive numbers
        and ending up with a negative result: 64 + 64 => -128). It is determined
        by looking at the carry between bits 6 and 7 and between bit 7 and the carry flag.
    */
    Byte Overflow : 1;

    /*  The negative flag is set if the result of the last operation had bit 7 set to a one. */
    Byte Negative : 1;

    u32 RunOneInstruction();

    void Reset();
    
private:

};