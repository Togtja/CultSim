#include "camera.h"

namespace cs
{
namespace gfx
{
void camera::turn(GLfloat angle)
{
    if (m_rotation + angle > 360.f)
    {
        m_rotation = (m_rotation + angle) - 360.f;
    }
    else if (m_rotation + angle < 0.f)
    {
        m_rotation = (m_rotation + angle) + 360.f;
    }
    else
    {
        m_rotation += angle;
    }
}
void camera::move(glm::uvec2)
{

}
void camera::zoom(float)
{
}
} // namespace gfx
} // namespace cs
