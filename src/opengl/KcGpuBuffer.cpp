#include "KcGpuBuffer.h"
#include <assert.h>
#include "glad.h"


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
    glBindBuffer(GL_ARRAY_BUFFER, handle());
}


void KcGpuBuffer::setData(const void* data, unsigned bytes, KeUsage usage)
{
    const static GLenum glUsages[] = {
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

    // we use the GL_ARRAY_BUFFER slot to send the data for no special reason
    GLuint last_array_buffer; 
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, handle()); 
    glBufferData(GL_ARRAY_BUFFER, bytes, data, glUsages[usage]);

    if (last_array_buffer != handle())
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer); // TODO: 是否这种恢复模式？

    bytes_ = bytes;
    usage_ = usage;
}


void KcGpuBuffer::setSubData(const void* data, unsigned bytes, int offset)
{
    assert(handle() && data);
    assert(bytes + offset <= bytesCount());

    GLuint last_array_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, handle()); 
    glBufferSubData(GL_ARRAY_BUFFER, offset, bytes, data); 

    if (last_array_buffer != handle())
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
}


void* KcGpuBuffer::map(KeAccess access)
{
    const static GLenum glAccess[] = {
        GL_READ_ONLY,
        GL_WRITE_ONLY,
        GL_READ_WRITE
    };

    create_();

    GLuint last_array_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, handle()); 
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, glAccess[access]);

    if (last_array_buffer != handle())
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);

    return ptr;
}


bool KcGpuBuffer::unmap()
{
    assert(handle());

    GLuint last_array_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, handle());

    bool ok = glUnmapBuffer(GL_ARRAY_BUFFER) == GL_TRUE;

    if (last_array_buffer != handle())
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);

    return true;
}
