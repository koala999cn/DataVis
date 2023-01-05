#pragma once


// 封装opengl的buffer object有关操作

class KcGpuBuffer
{
public:

    enum KeType
    {
        k_vertex_buffer, // vbo
        k_index_buffer // ibo
    };

    KcGpuBuffer(KeType type = k_vertex_buffer) : type_(type) {}

    ~KcGpuBuffer() { destroy(); }

    unsigned int handle() const { return handle_; }

    void bind() const;

    unsigned bytesCount() const { return bytes_; }

    void destroy();

    enum KeUsage
    {
        k_stream_draw,  // Data is specified once and used at most a few times as the source of drawing and image specification commands.
        k_stream_read,  // Data is copied once from an OpenGL buffer and is used at most a few times by the application as data values.
        k_stream_copy,  // Data is copied once from an OpenGL buffer and is used at most a few times as the source for drawing or image specification commands.
        k_static_draw,  // Data is specified once and used many times as the source of drawing or image specification commands.
        k_static_read,  // Data is copied once from an OpenGL buffer and is used many times by the application as data values.
        k_static_copy,  // Data is copied once from an OpenGL buffer and is used many times as the source for drawing or image specification commands.
        k_dynamic_draw, // Data is specified many times and used many times as the source of drawing and image specification commands.
        k_dynamic_read, // Data is copied many times from an OpenGL buffer and is used many times by the application as data values.
        k_dynamic_copy  // Data is copied many times from an OpenGL buffer and is used many times as the source for drawing or image specification commands.
    };

    // Modifies the BufferObject using the supplied data.
    // Use this function to initialize or resize the BufferObject and set it's usage flag.
    // If data == NULL the buffer will be allocated but no data will be written to the BufferObject.
    // If data != NULL such data will be copied into the BufferObject.
    void setData(const void* data, unsigned bytes, KeUsage usage);

    // Modifies an existing BufferObject using the supplied data.
    // @note You can use this function only on already initialized BufferObjects, use setBufferData() to initialize a BufferObject.
    // @param[in] data Must be != NULL, pointer to the data being written to the BufferObject.
    void setSubData(const void* data, unsigned bytes, int offset);

    enum KeAccess
    {
        k_read_only,
        k_write_only,
        k_read_write
    };

    // Maps a BufferObject so that it can be read or written by the CPU.
    // You can map only one BufferObject at a time and you must unmap it before using the BufferObject again or mapping another one.
    void* map(KeAccess access);


    // Unmaps a previously mapped BufferObject.
    // @return Returs true or false based on what's specified in the OpenGL specs:
    // "UnmapBuffer returns TRUE unless data values in the buffer's data store have
    // become corrupted during the period that the buffer was mapped. Such corruption
    // can be the result of a screen resolution change or other window system-dependent
    // event that causes system heaps such as those for high-performance graphics memory
    // to be discarded. GL implementations must guarantee that such corruption can
    // occur only during the periods that a buffer's data store is mapped. If such corruption
    // has occurred, UnmapBuffer returns FALSE, and the contents of the buffer's
    // data store become undefined."
    bool unmap();

private:

    void create_();

    unsigned int binding_() const;

    void bind_(unsigned int id) const;

private:
    KeType type_;
    unsigned int handle_{ 0 };
    unsigned bytes_{ 0 };
    KeUsage usage_{ k_static_draw };

};