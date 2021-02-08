#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class STY_ZeroPage_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x94;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 4;

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
        const Byte offset = 0x5f;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, 0xFF);
        cpu->Y = test_val;
        cpu->X = offset;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), test_val);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte zp_addr = 0xBA;
        const Byte offset = 0x22;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, 0x00);
        cpu->Y = test_val;
        cpu->X = offset;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), test_val);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte zp_addr = 0x08;
        const Byte offset = 0xF0;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, 0x00);
        cpu->Y = test_val;
        cpu->X = offset;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), test_val);
    }
};