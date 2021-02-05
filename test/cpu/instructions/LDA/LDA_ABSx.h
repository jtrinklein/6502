#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Absolute_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xBD;
    static constexpr Byte op_size = 3;
    static constexpr Byte op_cycles = 4;

    void setUp() {
        mem = new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Word addr = 0x5432;
        const Byte offset = 0x04;
        const Byte d[] = { opcode, 0,0, 0xFF, 0xFF, 0xFF };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);
        mem->WriteByte(addr + offset, test_val);

        cpu->X = offset;
        cpu->A = 0x11;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->X, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Word addr = 0x60D5;
        const Byte offset = 0x13;
        const Byte d[] = { opcode, 0,0, 0xff };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);
        mem->WriteByte(addr + offset, test_val);

        cpu->X = offset;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->X, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Word addr = 0x7802;
        const Byte offset = 0xF0;
        const Byte d[] = { opcode, 0,0, 0x7f };

        mem->LoadFromDataAtOffset(d, sizeof(d), addr);
        mem->WriteWord(addr + 1, addr);
        mem->WriteByte(addr + offset, test_val);

        cpu->X = offset;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->X, offset);
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

        cpu->X = offset;
        cpu->A = 0x00;
        cpu->PC = addr;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, val);
        TS_ASSERT_EQUALS(cycles, op_cycles + 1);
        TS_ASSERT_EQUALS(cpu->PC - addr, op_size);
        TS_ASSERT_EQUALS(cpu->X, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }
};