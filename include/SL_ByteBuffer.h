#ifndef SOCKETLITE_BYTEBUFFER_H
#define SOCKETLITE_BYTEBUFFER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#include "SL_Malloc.h"
#include "SL_Utility_Memory.h"
#include "SL_Sync_Atomic.h"

class SL_Buffer
{
public:
    SL_Buffer()
    {
    }

    virtual ~SL_Buffer()
    {
    }

    virtual void clear()
    {
    }

    virtual void reset()
    {
    }

    virtual char* buffer() const
    { 
        return NULL;
    }

    virtual uint buffer_size() const
    { 
        return 0;
    }

    virtual bool data(const char *data, uint size, bool owner)
    { 
        return false;
    }

    virtual char* data() const
    { 
        return NULL;
    }

    virtual uint data_size() const
    { 
        return 0;
    }

    //返回未使用的容量
    virtual uint free_size() const
    { 
        return 0; 
    }

    virtual uint data_begin() const
    { 
        return 0;
    }

    virtual void data_begin(uint new_begin_index)
    {
    }

    virtual uint data_end() const
    { 
        return 0;
    }

    virtual void data_end(uint new_end_index)
    {
    }

    virtual void advance(uint amount)
    {
    }

    virtual bool empty() const
    { 
        return true;
    }

    virtual bool owner() const
    { 
        return false;
    }

    virtual void owner(bool owner)
    {
    }

    virtual bool reserve(uint new_size)
    {
        return false;
    }

    virtual bool resize(uint new_size)
    { 
        return reserve(new_size);
    }

    virtual bool write(const char *data, uint size, uint multiple)
    {
        return false;
    }

    virtual bool write(const char *data)
    {
        return write(data, (uint)strlen(data), 100);
    }

    virtual bool write(char c)
    {
        return write((const char *)&c, sizeof(char), 100);
    }

    virtual bool write(short n)
    {
        return write((const char *)&n, sizeof(short), 100);
    }

    virtual bool write(int n)
    {
        return write((const char *)&n, sizeof(uint), 100);
    }

    virtual bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64), 100);
    }

    virtual bool write(float f)
    {
        return write((const char *)&f, sizeof(float), 100);
    }

    virtual bool write(double d)
    {
        return write((const char *)&d, sizeof(double), 100);
    }
};

class SL_ByteBuffer
{
public:
    inline SL_ByteBuffer()
        : buffer_(NULL)
        , buffer_size_(0)
        , data_begin_index_(0)
        , data_end_index_(0)
        , owner_(true)
    {
    }

    inline SL_ByteBuffer(uint buffer_size)
        : data_begin_index_(0)
        , data_end_index_(0)
        , owner_(true)
    {
        char *new_buffer = (char *)sl_malloc(buffer_size);
        if (NULL != new_buffer)
        {
            buffer_      = new_buffer;
            buffer_size_ = buffer_size;
        }
        else
        {
            buffer_      = NULL;
            buffer_size_ = 0;
        }
    }

    inline SL_ByteBuffer(const SL_ByteBuffer &buf)
    {
        if (buf.owner_)
        {
            buf.owner_          = false;
            buffer_             = buf.buffer_;
            buffer_size_        = buf.buffer_size_;
            data_begin_index_   = buf.data_begin_index_;
            data_end_index_     = buf.data_end_index_;
            owner_              = true;
        }
        else
        {
            buffer_             = NULL;
            buffer_size_        = 0;
            data_begin_index_   = 0;
            data_end_index_     = 0;
            owner_              = false;
        }
    }

    inline ~SL_ByteBuffer()
    {
        if (owner_ && (NULL != buffer_))
        {
            sl_free(buffer_);
        }
    }

    inline SL_ByteBuffer& operator= (const SL_ByteBuffer &buf)
    {
        if (this != &buf)
        {
            if (owner_ && (NULL != buffer_))
            {
                sl_free(buffer_);
            }

            if (buf.owner_)
            {
                buf.owner_          = false;
                buffer_             = buf.buffer_;
                buffer_size_        = buf.buffer_size_;
                data_begin_index_   = buf.data_begin_index_;
                data_end_index_     = buf.data_end_index_;
                owner_              = true;
            }
            else
            {
                buffer_             = NULL;
                buffer_size_        = 0;
                data_begin_index_   = 0;
                data_end_index_     = 0;
                owner_              = false;
            }
        }
        return *this;
    }

    inline void clear()
    {
        if (owner_ && (NULL != buffer_))
        {
            sl_free(buffer_);
        }
        buffer_             = NULL;
        buffer_size_        = 0;
        data_begin_index_   = 0;
        data_end_index_     = 0;
        owner_              = true;
    }

    inline void reset()
    {
        data_begin_index_   = 0;
        data_end_index_     = 0;
    }

    inline char* buffer() const
    {
        return buffer_;
    }

    inline uint buffer_size() const
    {
        return buffer_size_;
    }

    inline bool data(const char *data, uint size, bool owner = true)
    {
        if (size <= 0)
        {
            return false;
        }
        if (NULL == data)
        {
            return false;
        }
        if ( (data >= buffer_) && (data < (buffer_ + buffer_size_)) )
        {
            return false;
        }

        if (owner)
        {
            char *new_buffer;
            if (owner_)
            {
                new_buffer = (char *)sl_realloc(buffer_, size);
            }
            else
            {   
                new_buffer = (char *)sl_malloc(size);
            }
            if (NULL == new_buffer)
            {
                return false;
            }
            sl_memcpy(new_buffer, data, size);
            buffer_ = new_buffer;
        }
        else
        {
            if (owner_ && (buffer_ != NULL))
            {
                sl_free(buffer_);
            }
            buffer_ = (char *)data;
        }
        buffer_size_        = size;
        data_begin_index_   = 0;
        data_end_index_     = size;
        owner_              = owner;
        return true;
    }

    inline char* data() const
    {
        return buffer_ + data_begin_index_;
    }

    inline uint data_size() const
    {
        return data_end_index_ - data_begin_index_;
    }

    //返回未使用的容量
    inline uint free_size() const
    {
        return buffer_size_ - data_end_index_;
    }

    inline uint data_begin() const
    { 
        return data_begin_index_;
    }

    inline void data_begin(uint new_begin_index)
    {
        if (new_begin_index > buffer_size_)
        {
            new_begin_index = buffer_size_;
        }
        data_begin_index_ = new_begin_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_end_index_ = data_begin_index_;
        }
    }

    inline uint data_end() const
    {
        return data_end_index_;
    }

    inline void data_end(uint new_end_index)
    {
        if (new_end_index > buffer_size_)
        {
            new_end_index = buffer_size_;
        }
        data_end_index_ = new_end_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_begin_index_ = data_end_index_;
        }
    }

    inline void advance(uint amount)
    {
        if (buffer_size_ >= (data_end_index_ + amount))
        {
            data_end_index_ += amount;
        }
    }

    inline bool empty() const
    {
        return data_end_index_ - data_begin_index_ == 0;
    }

    inline bool owner() const
    {
        return owner_;
    }

    inline void owner(bool owner)
    {
        owner_ = owner;
    }

    inline bool reserve(uint new_size)
    {
        if (owner_)
        {
            if (new_size > buffer_size_)
            {
                char *new_buffer = (char *)sl_realloc(buffer_, new_size);
                if (NULL != new_buffer)
                {
                    buffer_             = new_buffer;
                    buffer_size_        = new_size;
                    data_begin_index_   = 0;
                    data_end_index_     = 0;
                    return true;
                }
            }
        }
        else
        {
            char *new_buffer = (char *)sl_malloc(new_size);
            if (NULL != new_buffer)
            {
                buffer_             = new_buffer;
                buffer_size_        = new_size;
                data_begin_index_   = 0;
                data_end_index_     = 0;
                owner_              = true;
                return true;
            }
        }
        return false;
    }

    inline bool resize(uint new_size)
    {
        return reserve(new_size);
    }

    //multiple: 缓冲区扩大倍数
    inline bool write(const char *data, uint size, uint multiple = 100)
    {
        if (!owner_)
        {
            return false;
        }

        if (free_size() >= size)
        {
            sl_memcpy(buffer_ + data_end_index_, data, size);
            data_end_index_ += size;
        }
        else
        {   
            return adjust_buffer_i(data, size, multiple);
        }
        return true;
    }

    inline bool write(const char *data)
    {
        return write(data, (strlen(data) + 1));
    }

    inline bool write(char c)
    {
        return write((const char *)&c, sizeof(char));
    }

    inline bool write(short n)
    {
        return write((const char *)&n, sizeof(short));
    }

    inline bool write(int n)
    {
        return write((const char *)&n, sizeof(uint));
    }

    inline bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64));
    }

    inline bool write(float f)
    {
        return write((const char *)&f, sizeof(float));
    }

    inline bool write(double d)
    {
        return write((const char *)&d, sizeof(double));
    }

    inline uint read(char *data, uint size)
    {
        if (data_size() >= size)
        {
            sl_memcpy(data, buffer_ + data_begin_index_, size);
            data_begin_index_ += size;
            return size;
        }
        return -1;
    }

    inline char* read(uint size)
    {
        if (data_size() >= size)
        {
            char *ret = (buffer_ + data_begin_index_);
            data_begin_index_ += size;
            return ret;
        }
        return NULL;
    }

private:
    inline bool adjust_buffer_i(const char *data, uint size, uint multiple)
    {
        uint  new_size    = buffer_size_ * multiple / 100 + buffer_size_ + size;
        char *new_buffer = (char *)sl_realloc(buffer_, new_size);
        if (NULL != new_buffer)
        {
            sl_memcpy(new_buffer + data_end_index_, data, size);
            buffer_         = new_buffer;
            buffer_size_    = new_size;
            data_end_index_ += size;
            return true;
        }
        return false;
    }

private:
    char            *buffer_;           //缓冲区
    uint            buffer_size_;       //缓冲区大小
    uint            data_begin_index_;  //数据开始位
    uint            data_end_index_;    //数据结束位
    mutable bool    owner_;             //表示是否拥有所有权
};

class SL_SharedBuffer
{
public:
    inline SL_SharedBuffer()
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buffer_ = new Buffer_();
    }

    inline SL_SharedBuffer(uint buffer_size)
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buffer_ = new Buffer_(buffer_size);
    }

    inline SL_SharedBuffer(const SL_SharedBuffer &buf)
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buf.buffer_->increment_refcount();
        buffer_             = buf.buffer_;
        data_begin_index_   = buf.data_begin_index_;
        data_end_index_     = buf.data_end_index_;
    }

    inline ~SL_SharedBuffer()
    {
        buffer_->release();
    }

    inline SL_SharedBuffer& operator=(const SL_SharedBuffer &buf)
    {
        buffer_->release();

        buf.buffer_->increment_refcount();
        buffer_             = buf.buffer_;
        data_begin_index_   = buf.data_begin_index_;
        data_end_index_     = buf.data_end_index_;
        return *this;
    }

    inline void clear()
    {
        reset();
        buffer_->clear();
    }

    inline bool reserve(uint new_size)
    {
        return buffer_->reserve(new_size);
    }

    inline bool resize(uint new_size)
    {
        return reserve(new_size);
    }

    inline void reset()
    {
        data_begin_index_ = 0;
        data_end_index_   = 0;
    }

    inline uint use_count()
    {
        return buffer_->refcount_.load();
    }

    inline bool empty() const
    {
        return data_end_index_ - data_begin_index_ == 0;
    }

    inline uint buffer_size() const
    {
        return buffer_->buffer_size();
    }

    inline char* buffer()
    {
        return buffer_->buffer();
    }

    //返回未使用的容量
    inline uint free_size() const
    {
        return buffer_->free_size();
    }

    inline bool data(const char *data, uint size, bool owner = true)
    {
        if (size <= 0)
        {
            return false;
        }
        if (NULL == data)
        {
            return false;
        }
        buffer_->release();
        reset();

        if (owner)
        {
            buffer_ = new Buffer_(size);
            write(data, size);
        }
        else
        {
            buffer_ = new Buffer_();
            buffer_->buffer_ = (char *)data;
            buffer_->buffer_size_ = size;
        }
        buffer_->owner_ = owner;
        return true;
    }

    inline char* data() const
    {
        return buffer_->buffer_ + data_begin_index_;
    }

    inline uint data_size() const
    {
        return data_end_index_ - data_begin_index_;
    }

    inline uint data_begin() const
    { 
        return data_begin_index_;
    }

    inline void data_begin(uint new_begin_index)
    {
        if (new_begin_index > buffer_->buffer_size_)
        {
            new_begin_index = buffer_->buffer_size_;
        }
        data_begin_index_ = new_begin_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_end_index_ = data_begin_index_;
        }
    }

    inline uint data_end() const
    {
        return data_end_index_;
    }

    inline void data_end(uint new_end_index)
    {
        if (new_end_index > buffer_->buffer_size_)
        {
            new_end_index = buffer_->buffer_size_;
        }
        data_end_index_ = new_end_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_begin_index_ = data_end_index_;
        }
    }

    inline void advance(uint amount)
    {
        if (buffer_->buffer_size() >= (data_end_index_ + amount))
        {
            data_end_index_ += amount;
        }
    }

    inline bool owner() const
    {
        return buffer_->owner_;
    }

    inline void owner(bool owner)
    {
        buffer_->owner_ = owner;
    }

    //multiple: 缓冲区扩大倍数
    inline bool write(const char *data, uint size, uint multiple = 100)
    {
        bool ret = buffer_->write(data, size, multiple);
        if (ret)
        {
            data_end_index_ += size;
        }
        return ret;
    }

    inline bool write(const char *data)
    {
        return write(data, (uint)(strlen(data) + 1));
    }

    inline bool write(char c)
    {
        return write((const char *)&c, sizeof(char));
    }

    inline bool write(short n)
    {
        return write((const char *)&n, sizeof(short));
    }

    inline bool write(int n)
    {
        return write((const char *)&n, sizeof(uint));
    }

    inline bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64));
    }

    inline bool write(float f)
    {
        return write((const char *)&f, sizeof(float));
    }

    inline bool write(double d)
    {
        return write((const char *)&d, sizeof(double));
    }

    inline uint read(char *data, uint size)
    {
        uint data_size = data_end_index_ - data_begin_index_;
        if (data_size >= size)
        {
            sl_memcpy(data, buffer_->buffer_ + data_begin_index_, size);
            data_begin_index_ += size;
            return size;
        }
        return -1;
    }

    inline char* read(uint size)
    {
        uint data_size = data_end_index_ - data_begin_index_;
        if (data_size >= size)
        {
            char *ret = buffer_->buffer_ + data_begin_index_;
            data_begin_index_ += size;
            return ret;
        }
        return NULL;
    }

private:
    class Buffer_
    {
    public:
        inline Buffer_()
            : buffer_(NULL)
            , buffer_size_(0)
            , use_size_(0)
            , refcount_(1)
            , owner_(true)
        {
        }

        inline Buffer_(uint buffer_size)
            : use_size_(0)
            , refcount_(1)
            , owner_(true)
        {
            char *new_buffer = (char *)sl_malloc(buffer_size);
            if (NULL != new_buffer)
            {
                buffer_      = new_buffer;
                buffer_size_ = buffer_size;
            }
            else
            {
                buffer_      = NULL;
                buffer_size_ = 0;
            }
        }

        inline ~Buffer_()
        {
        }

        inline bool reserve(uint new_size)
        {   
            if (owner_ && (new_size > buffer_size_))
            {
                char *new_buffer = (char *)sl_realloc(buffer_, new_size);
                if (NULL != new_buffer)
                {
                    buffer_      = new_buffer;
                    buffer_size_ = new_size;
                    return true;
                }
            }
            return false;
        }

        //加1, 返回变化后值
        inline int32 increment_refcount()
        {
            return ++refcount_;
        }

        //减1, 返回变化后值
        inline int32 decrement_refcount()
        {
            return --refcount_;
        }

        inline void clear()
        {
            if (NULL != buffer_)
            {
                if (owner_)
                {
                    sl_free(buffer_);
                }
                buffer_ = NULL;
                buffer_size_ = 0;
                use_size_ = 0;
            }
        }

        inline void release()
        {
            if (--refcount_ == 0)
            {
                if (NULL != buffer_)
                {
                    if (owner_)
                    {
                        sl_free(buffer_);
                    }
                    buffer_ = NULL;
                    buffer_size_ = 0;
                    use_size_ = 0;
                }

                delete this;
            }
        }

        inline uint buffer_size() const
        {
            return buffer_size_;
        }

        inline char* buffer()
        {
            return buffer_;
        }

        inline uint free_size() const
        {
            return buffer_size_ - use_size_;
        }

        //multiple: 缓冲区扩大倍数
        inline bool write(const char *data, uint size, uint multiple = 100)
        {
            if (free_size() >= size)
            {
                sl_memcpy(buffer_ + use_size_, data, size);
                use_size_ += size;
                return true;
            }
            else
            {
                if (owner_)
                {
                    uint new_size  = buffer_size_ * multiple / 100 + buffer_size_ + size;
                    char *new_buffer = (char *)sl_realloc(buffer_, new_size);
                    if (NULL != new_buffer)
                    {
                        sl_memcpy(new_buffer + use_size_, data, size);
                        use_size_      += size;
                        buffer_size_    = new_size;
                        buffer_         = new_buffer;
                        return true;
                    }
                }
            }
            return false;
        }

    public:
        char                    *buffer_;       //缓冲区
        uint                    buffer_size_;   //缓冲区大小
        uint                    use_size_;      //已使用大小
        SL_Sync_Atomic_Int32    refcount_;      //引用计数
        bool                    owner_;         //表示是否拥有所有权
    };

private:
    Buffer_ *buffer_;       
    uint    data_begin_index_;  //数据开始位
    uint    data_end_index_;    //数据结束位
};

#endif

