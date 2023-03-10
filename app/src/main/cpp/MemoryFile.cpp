//
// Created by 15789 on 2022/12/29.
//
#include <cstdint>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <mutex>
#include "MemoryFile.h"

std::mutex mtx;

//系统给我们提供真正的内存时，以页为单位提供
//内存分页大小
int32_t DEFAULT_FILE_SIZE = getpagesize();

MemoryFile::MemoryFile(const char *path){
    m_path = path;
    m_fd = open(m_path, O_RDWR | O_CREAT, S_IRWXU);
    m_size = DEFAULT_FILE_SIZE;
    //将文件设置为m_size大小
    ftruncate(m_fd, m_size);
    //mmap内存映射
    m_ptr = static_cast<int8_t *>(mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
    //初始化m_actualSize为0
    m_actualSize = 0;
}

MemoryFile::~MemoryFile() {
    munmap(m_ptr, m_size);
    close(m_fd);
}

void MemoryFile::write(char *data, int dataLen) {
    mtx.lock();
    if (m_actualSize + dataLen >= m_size) {
        resize(m_actualSize+dataLen);
    }
    //将data的dataLen长度的数据，拷贝到m_ptr+m_actualSize
    memcpy(m_ptr+m_actualSize, data, dataLen);//操作内存，通过内存映射
    //重设最初位置
    m_actualSize += dataLen;
    mtx.unlock();
}

void MemoryFile::resize(int32_t needSize) {
    int32_t oldSize = m_size;
    do{
        m_size *= 2;
    } while (m_size < needSize);
    //设置文件大小
    ftruncate(m_fd, m_size);
    //解除映射
    munmap(m_ptr, oldSize);
    //重新进行mmap内存映射
    m_ptr = static_cast<int8_t *>(mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
}