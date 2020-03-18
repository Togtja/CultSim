#pragma once
#include "glutil.h"

#include <array>
#include <utility>
#include <type_traits>

#include <glad/glad.h>

namespace cs::gfx
{
/**
 * Wrapper around an OpenGL Uniform Buffer with the purpose of storing larger data blocks for shaders
 */
template<typename T, std::size_t Num = 1>
class UniformBuffer
{
public:
    using ValueType = T;

private:
    /** The CPU side value of the buffer, must be flushed to GPU after updates */
    std::array<T, Num> m_value{};

    /** Pointer to the underlying data if the buffer can be mapped */
    T* m_mapped_pointer = nullptr;

    /** OpenGL Handle */
    uint32_t m_handle{};

public:
    UniformBuffer()
    {
        initialize();
        flush();
    }

    UniformBuffer(const T& initial_value) : m_value({initial_value})
    {
        initialize();
        flush();
    }

    UniformBuffer(const std::array<T, Num>& initial_values) : m_value(initial_values)
    {
        static_assert(Num > 1, "must be array to use array initializer");
        initialize();
        flush();
    }

    UniformBuffer(const UniformBuffer<T>& other) = delete;
    UniformBuffer& operator=(const UniformBuffer<T>& other) = delete;

    UniformBuffer& operator=(UniformBuffer<T>&& other)
    {
        if (&other == this)
        {
            return *this;
        }

        /** Destroy */
        deinitialize();

        /** Steal */
        m_value          = std::move(other.m_value);
        m_mapped_pointer = other.m_mapped_pointer;
        m_handle         = other.m_handle;

        /** Clean up */
        other.m_mapped_pointer = nullptr;
        other.m_handle         = 0u;

        return *this;
    }

    UniformBuffer(UniformBuffer<T>&& other) :
        m_value(std::move(other.m_value)),
        m_mapped_pointer(other.m_mapped_pointer),
        m_handle(other.m_handle)
    {
        other.m_handle         = 0u;
        other.m_mapped_pointer = nullptr;
    }

    /**
     * Flush the contents of the uniform buffer to make it available for the GPU
     */
    void flush()
    {
        memcpy(m_mapped_pointer, m_value.data(), size_bytes(m_value));
        glFlushMappedNamedBufferRange(m_handle, 0, size_bytes(m_value));
    }

    /**
     * Flush the contents of the uniform buffer to make it available for the GPU
     *
     * @param values Set the value before flushing
     */
    void flush(const T& value)
    {
        m_value[0] = value;
        flush();
    }

    /**
     * Flush the contents of the uniform buffer to make it available for the GPU
     *
     * @param values Set the values before flushing
     */
    void flush(const std::array<T, Num>& values)
    {
        static_assert(Num > 1, "can not use array version on non-array data");
        m_value = values;
        flush();
    }

    /**
     * Bind the buffer to the given uniform buffer binding point
     *
     * @param bind_point The Uniform buffer binding point to bind to
     */
    void bind(unsigned bind_point)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, bind_point, m_handle, 0, size_bytes(m_value));
    }

private:
    void initialize()
    {
        glCreateBuffers(1, &m_handle);
        glNamedBufferStorage(m_handle, size_bytes(m_value), m_value.data(), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        m_mapped_pointer =
            static_cast<T*>(glMapNamedBufferRange(m_handle,
                                                  0,
                                                  size_bytes(m_value),
                                                  GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
    }

    void deinitialize()
    {
        glUnmapNamedBuffer(m_handle);
        glDeleteBuffers(1, &m_handle);
        m_mapped_pointer = nullptr;
        m_handle         = 0u;
    }
};

} // namespace cs::gfx
