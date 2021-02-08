#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class STA_Absolute_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x9D;
    static constexpr Byte op_size = 3;
    static constexpr Byte op_cycles = 5;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Word addr = 0x5432;
        const Byte offset = 0x5f;
        const Byte d[] = {opcode, 0,0, 0xFF};

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(1, addr);

        cpu->X = offset;
        cpu->A = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(mem->ReadByte(addr+offset), test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0xFA;
        const Word addr = 0x8FD5;
        const Byte offset = 0xFF;
        const Byte d[] = {opcode, 0,0 , 0x02};

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(1, addr);

        cpu->X = offset;
        cpu->A = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(mem->ReadByte(addr+offset), test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
    }
};