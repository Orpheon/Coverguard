#pragma once

#include "global.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

class Buffer
{
    public:
        Buffer(void *data_, uint64_t datalen_);
        virtual ~Buffer();
        void* getdata() {return data;}
        virtual uint64_t length() = 0;
        void reset() {pos = 0;}
    protected:
        void *data;
        uint64_t datalen;
        uint64_t pos;
    private:
};

class ReadBuffer : public Buffer
{
    public:
        ReadBuffer(void *data_, uint64_t datalen_);
        virtual ~ReadBuffer() override;
        template<class T> T read()
        {
            if (datalen-pos < sizeof(T))
            {
                // Pulling too much data
                Global::logging().panic(__FILE__, __LINE__, "Attempted to pull %i bytes from a readbuffer, but only %i are left", sizeof(T), datalen-pos);
            }
            T r = (*reinterpret_cast<T*>(reinterpret_cast<char*>(data)+pos));
            pos += sizeof(T);
            return r;
        }
        uint64_t length() override {return datalen-pos;}
    protected:
    private:
};

class WriteBuffer : public Buffer
{
    public:
        WriteBuffer();
        virtual ~WriteBuffer() override;
        void enlarge(uint64_t newsize);
        template<class T> void write(T input)
        {
            if (datalen-pos < sizeof(T))
            {
                // Buffer too small, resize
                enlarge(datalen*2.0);
            }
            std::memcpy(reinterpret_cast<char*>(data)+pos, &input, sizeof(T));
            pos += sizeof(T);
        }
        uint64_t length() override {return pos;}
    protected:
    private:
};
