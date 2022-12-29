//
// Created by 15789 on 2022/12/29.
//
#include <cstdint>

class MemoryFile {
public:
    MemoryFile(const char *path);

    ~MemoryFile();

    void write(char *data, int dataLen);

private:
    void resize(int32_t needSize);

private:
    const char *m_path;
    int m_fd;
    int32_t m_size;
    int8_t *m_ptr;
    int m_actualSize;
};