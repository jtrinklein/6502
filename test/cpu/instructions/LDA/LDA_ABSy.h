#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Absolute_Y_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xB9;
    static constexpr Byte op_cycles = 4;
    static constexpr Byte op_size = 3;

    void setUp() {
        mem = new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte zero_val = 0x00;
        const Word addr = 0x5432;
        const Byte offset = 0x04;
        const Byte d[] = {
            opcode, 0,0,
            0x0F,zero_val,0xFF
        };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);

        cpu->Y = offset;
        cpu->A = 0x11;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, zero_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte nonzero_val = 0x67;
        const Word addr = 0x60D5;
        const Byte offset = 0x06;
        const Byte d[] = {
            opcode, 0,0,
            0xff, 0xff, 0xff, nonzero_val, 0xff
        };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);

        cpu->Y = offset;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, nonzero_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte neg_val = 0xE7;
        const Word addr = 0x7878;
        const Byte d[] = {
            opcode, 0,0,
            neg_val, 0x7f, 0x7f 
        };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);

        cpu->Y = 0x03;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, neg_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->Y, 0x03);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_WithPageCross( void ) {
        const Byte val = 0x2C;
        const Word addr = 0x78FC;
        const Byte offset = 0x07;
        const Byte d[] = {
            opcode, 0,0,
            0xFF, //78FF
            0xFF, //7900
            0xFF, //7901
            0xFF, //7902
            val, //7903
            0xFF
        };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);

        cpu->Y = offset;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, val);
        TS_ASSERT_EQUALS(cycles, op_cycles + 1);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }
};