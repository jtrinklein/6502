#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Absolute_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xAD;
    static constexpr Byte op_cycles = 4;
    static constexpr Byte op_size = 3;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Word addr = 0x5432;
        const Word pc_start = addr + 0x0002;
        const Byte d[] = {test_val, 0x01, opcode, 0,0, 0xFF};
        mem->LoadFromDataAtOffset(d, sizeof(d), addr);

        mem->WriteWord(pc_start + 1, addr);
        cpu->PC = pc_start;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Word addr = 0x60D5;
        const Word pc_start = addr + 0x0002;
        const Byte d[] = {test_val, 0x1, opcode, 0,0 , 0x02};

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(pc_start + 1, addr);

        cpu->PC = pc_start;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Word addr = 0x8888;
        const Word pc_start = addr + 0x0002;

        const Byte d[] = { test_val, 0x01, opcode, 0,0 , 0x7f };
        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(pc_start + 1, addr);

        cpu->PC = pc_start;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
    
};