
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

namespace cs
{
namespace gfx
{
class Camera
{
private:
    glm::vec2 m_pos;
    GLfloat m_fov;
    GLfloat m_rotation;
    GLfloat m_speed;

public:
    void init(glm::vec2);
    glm::mat4 get_view_matrix();

    void turn(GLfloat);
    void move(glm::vec2);
    void zoom(float);


};
} // namespace gfx
} // namespace cs
