#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class STY_ZeroPage_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x84;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 3;

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
        const Byte zp_addr = 0x12;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr, 0xFF);
        cpu->Y = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte zp_addr = 0xFA;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr, 0x00);
        cpu->Y = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte zp_addr = 0x08;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr, 0x00);
        cpu->Y = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);
    }
};