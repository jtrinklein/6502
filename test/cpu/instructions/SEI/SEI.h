#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class SEI_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x78;
    static constexpr Byte op_size = 1;
    static constexpr Byte op_cycles = 2;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_PreviouslyUnset( void ) {
        const Byte d[] = {opcode, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        cpu->InterruptDisable = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->InterruptDisable, 1);
    }

    void test_PreviouslySet( void ) {
        const Byte d[] = {opcode, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        cpu->InterruptDisable = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->InterruptDisable, 1);
    }
};