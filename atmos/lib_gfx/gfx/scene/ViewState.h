
#pragma once

namespace gfx
{
    class view_state
    {
    public:
        
        void near(float);
        
        void far(float);
        
        void viewport(const utility::viewport&);
        
        void eye(const glm::vec3&);
        
        void eye_forward(const glm::vec3&);
        
        void view_matrix(const glm::mat4x4&);
        
        void proj_matrix(const glm::mat4x4&);
        
        void view_proj_matrix(const glm::mat4x4&);

        void inv_view_proj_matrix(const glm::mat4x4&);
        
        void frustum(const frustum&);
        
        
        float near() const;
        
        float far() const;
        
        const utility::viewport& viewport() const;
        
        const glm::vec3& eye() const;
        
        const glm::vec3& eye_forward() const;
        
        const glm::mat4x4& view_matrix() const;
        
        const glm::mat4x4& proj_matrix() const;
        
        const glm::mat4x4& view_proj_matrix() const;

        const glm::mat4x4& inv_view_proj_matrix() const;
        
        const frustum& frustum() const;
        
    private:
        
        
        float _near = 0.1f;
        
        float _far = 1000.0f;
        
		utility::viewport _viewport;
        
        glm::vec3 _eye, _eye_forward;
        
        glm::mat4x4 _view_matrix, _proj_matrix, _view_proj_matrix, _inv_view_proj_matrix;
        
        frustum _frustum;
    };
}