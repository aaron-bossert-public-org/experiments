#include <gfx/scene/view_state.h>

using namespace gfx;

void view_state::near(float near)
{
    _near = near;
}

void view_state::far(float far)
{
    _far = far;
}

void view_state::viewport(const utility::viewport& viewport)
{
    _viewport = viewport;
}

void view_state::eye(const glm::vec3& eye)
{
    _eye = eye;
}

void view_state::eye_forward(const glm::vec3& eye_forward)
{
    _eye_forward = eye_forward;
}

void view_state::view_matrix(const glm::mat4x4& view_matrix)
{
    _view_matrix = view_matrix;
}

void view_state::proj_matrix(const glm::mat4x4& proj_matrix)
{
    _proj_matrix = proj_matrix;
}

void view_state::view_proj_matrix(const glm::mat4x4& view_proj_matrix)
{
    _view_proj_matrix = view_proj_matrix;
}

void view_state::inv_view_proj_matrix(const glm::mat4x4& inv_view_proj_matrix)
{
    _inv_view_proj_matrix = inv_view_proj_matrix;
}

void view_state::frustum(const frustum& frustum)
{
    _frustum = frustum;
}

float view_state::near() const
{
    return _near;
}

float view_state::far() const
{
    return _far;
}

const utility::viewport& view_state::viewport() const
{
    return _viewport;
}

const glm::vec3& view_state::eye() const
{
    return _eye;
}

const glm::vec3& view_state::eye_forward() const
{
    return _eye_forward;
}

const glm::mat4x4& view_state::view_matrix() const
{
    return _view_matrix;
}

const glm::mat4x4& view_state::proj_matrix() const
{
    return _proj_matrix;
}

const glm::mat4x4& view_state::view_proj_matrix() const
{
    return _view_proj_matrix;
}

const glm::mat4x4& view_state::inv_view_proj_matrix() const
{
    return _inv_view_proj_matrix;
}

const frustum& view_state::frustum() const
{
    return _frustum;
}
