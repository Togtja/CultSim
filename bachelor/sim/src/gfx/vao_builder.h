#pragma once

#include <functional>

namespace cs::gfx
{
/**
 * Utility class to create Vertex Array Objects with a builder like interface
 *
 * You can omit the vao parameter in all functions, otherwise they work mostly like the GL functions themselves, except that they
 * also automatically enable all created attributes, and you set the buffer binding directly when you declare the attribute, so
 * you save some space.
 */
class VaoBuilder
{
private:
    unsigned m_vao{};

public:
    VaoBuilder();
    VaoBuilder(const VaoBuilder&) = delete;
    VaoBuilder(VaoBuilder&&)      = delete;
    VaoBuilder& operator=(const VaoBuilder&) = delete;
    VaoBuilder& operator=(VaoBuilder&&) = delete;
    ~VaoBuilder();

    /**
     * Add a vertex attribute
     *
     * @param index The generic vertex attribute array being described.
     * @param binding The index of the vertex buffer binding with which to associate the generic vertex attribute.
     * @param size The number of values per vertex that are stored in the array.
     * @param type The type of the data stored in the array.
     * @param offset The offset, in bytes of the first element relative to the start of the vertex buffer binding this attribute
     * fetches from.
     * @param normalized GL_TRUE if the parameter represents a normalized integer (type must be an integer type). GL_FALSE
     * otherwise.
     */
    VaoBuilder& attribute(unsigned index, unsigned binding, int size, unsigned type, unsigned offset, bool normalized = false);

    /**
     * Add a vertex integer attribute
     *
     * @param index The generic vertex attribute array being described.
     * @param binding The index of the vertex buffer binding with which to associate the generic vertex attribute.
     * @param size The number of values per vertex that are stored in the array.
     * @param type The type of the data stored in the array.
     * @param offset The offset, in bytes of the first element relative to the start of the vertex buffer binding this attribute
     * fetches from.
     */
    VaoBuilder& iattribute(unsigned index, unsigned binding, int size, unsigned type, unsigned offset);

    /**
     * Associate a vertex buffer with a buffer binding slot for this vao
     *
     * @param vbo Handle to VBO that will be bound to this buffer binding when you use this VAO
     * @param binding The index of the vertex buffer binding with which to associate the vbo
     * @param offset The offset of the first element of the buffer.
     * @param stride The distance between elements within the buffer.
     */
    VaoBuilder& vbo(unsigned vbo, unsigned binding, unsigned offset = 0u, unsigned stride = 0u);

    /**
     * Associate an element buffer with this vao
     *
     * @param ebo Handle to EBO that will be used with this vao
     */
    VaoBuilder& ebo(unsigned ebo);

    /**
     * Set an attribute divisor for the given buffer binding, normally used with instancing
     *
     * @param binding The index of the binding whose divisor to modify.
     * @param divisor The new value for the instance step rate to apply.
     * @return
     */
    VaoBuilder& divisor(unsigned binding, unsigned divisor);

    /**
     * Bind the vao to the OpenGL State
     */
    VaoBuilder& bind();

    /**
     * Build the VAO and obtain ownership of the handle
     *
     * @warning You must clean up the returned resource
     * @return Handle to the created OpenGL Vertex Array Object
     */
    unsigned build();
};
} // namespace cs
