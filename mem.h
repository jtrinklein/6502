#pragma once
#include <string>
#include "types.h"

class Mem {
public:

    static constexpr size_t max_mem_size = 0x10000;

    Mem();
    ~Mem();

    void LoadFromFile(std::string path);
    void LoadFromData(const Byte* data, size_t num_bytes);
    void LoadFromDataAtOffset(const Byte* data, size_t num_bytes, size_t offset);
    void Unload();

    Byte ReadByte(Word addr);
    Byte WriteByte(Word addr, Byte data);
    Word ReadWord(Word addr);
    Word WriteWord(Word addr, Word data);

    bool m_log_enabled;
    Byte* m_data;
private:
    
    Byte ReadByteInternal(Word addr);
    Byte WriteByteInternal(Word addr, Byte data);
};
