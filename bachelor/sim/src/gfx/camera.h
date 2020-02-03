
#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace cs
{
namespace gfx
{
class camera
{
private:
    glm::uvec2 m_pos;
    GLfloat m_fov;
    GLfloat m_rotation;

public:
    void turn(GLfloat);
    void move(glm::uvec2);
    void zoom(float);
};
} // namespace gfx
} // namespace cs
