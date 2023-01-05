#include "KcGpuBuffer.h"
#include <assert.h>
#include "glad.h"


namespace kPrivate
{
    GLenum oglBufferType(KcGpuBuffer::KeType type)
    {
        constexpr static GLenum oglType[] = {
            GL_ARRAY_BUFFER,
            GL_ELEMENT_ARRAY_BUFFER
        };

        return oglType[type];
    }
}


void KcGpuBuffer::create_()
{
    if (handle() == 0) {
        assert(bytesCount() == 0);
        glGenBuffers(1, &handle_);
        bytes_ = 0;
    }
}


void KcGpuBuffer::destroy()
{
    if (handle() != 0) {
        glDeleteBuffers(1, &handle_); 
        handle_ = 0;
        bytes_ = 0;
    }
}


void KcGpuBuffer::bind() const
{
    bind_(handle());
}


void KcGpuBuffer::setData(const void* data, unsigned bytes, KeUsage usage)
{
    constexpr static GLenum glUsages[] = {
        GL_STREAM_DRAW,  
        GL_STREAM_READ,   
        GL_STREAM_COPY,   
        GL_STATIC_DRAW,   
        GL_STATIC_READ,   
        GL_STATIC_COPY,   
        GL_DYNAMIC_DRAW, 
        GL_DYNAMIC_READ, 
        GL_DYNAMIC_COPY
    };

    create_();

   auto lastBinding = binding_();

   bind();

    glBufferData(kPrivate::oglBufferType(type_), bytes, data, glUsages[usage]);

    if (lastBinding != handle())
        bind_(lastBinding); // TODO: 是否这种恢复模式？

    bytes_ = bytes;
    usage_ = usage;
}


void KcGpuBuffer::setSubData(const void* data, unsigned bytes, int offset)
{
    assert(handle() && data);
    assert(bytes + offset <= bytesCount());

    auto lastBinding = binding_();

    bind();

    glBufferSubData(kPrivate::oglBufferType(type_), offset, bytes, data); 

    if (lastBinding != handle())
        bind_(lastBinding); 
}


void* KcGpuBuffer::map(KeAccess access)
{
    constexpr static GLenum glAccess[] = {
        GL_READ_ONLY,
        GL_WRITE_ONLY,
        GL_READ_WRITE
    };

    create_();

    auto lastBinding = binding_();

    bind();

    void* ptr = glMapBuffer(kPrivate::oglBufferType(type_), glAccess[access]);

    if (lastBinding != handle())
        bind_(lastBinding);

    return ptr;
}


bool KcGpuBuffer::unmap()
{
    assert(handle());

    auto lastBinding = binding_();

    bind();

    bool ok = glUnmapBuffer(kPrivate::oglBufferType(type_)) == GL_TRUE;

    if (lastBinding != handle())
        bind_(lastBinding);

    return true;
}


unsigned int KcGpuBuffer::binding_() const
{
    constexpr static GLenum bindingFlag[] = {
        GL_ARRAY_BUFFER_BINDING,
        GL_ELEMENT_ARRAY_BUFFER_BINDING
    };

    GLuint last_binding;
    glGetIntegerv(bindingFlag[type_], (GLint*)&last_binding);

    return last_binding;
}


void KcGpuBuffer::bind_(unsigned int id) const
{
    glBindBuffer(kPrivate::oglBufferType(type_), id);
}
