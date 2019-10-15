
#include <gl/buffer/gl_compute_buffer.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end

using namespace igpu;

std::unique_ptr<gl_compute_buffer> gl_compute_buffer::make(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "invalid usage:%d", (int)cfg.usage);
	}
	else
	{
		return std::unique_ptr<gl_compute_buffer>(new gl_compute_buffer(cfg));
	}

	return nullptr;
}

gl_compute_buffer::gl_compute_buffer(const config& cfg)
	: gl_buffer_t (
		cfg,
		"GPU Compute Mem",
		cfg.usage,
		GL_UNIFORM_BUFFER)
{}