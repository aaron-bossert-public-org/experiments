
// #include <igpu/texture/sampler.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// // Vulkan implementation includes - end
// using namespace igpu;


// unsigned to_gl(sampler::filter filter)
// {
// 	switch (filter)
// 	{
// 	case sampler::filter::NEAREST:
// 		return GL_NEAREST;
// 	case sampler::filter::LINEAR:
// 		return GL_LINEAR;
// 	default:
// 		LOG_CONTEXT(CRITICAL,
// 			"unhandled type(%s)",
// 			sampler::to_string(filter).data());
// 		return GL_LINEAR;
// 	}
// }

// unsigned to_gl(sampler::address address)
// {
// 	switch (address)
// 	{
// 	case sampler::address::CLAMP:
// 		return GL_CLAMP_TO_EDGE;
// 	case sampler::address::WRAP:
// 		return GL_REPEAT;
// 	case sampler::address::MIRROR:
// 		return GL_MIRRORED_REPEAT;
// 	default:
// 		LOG_CONTEXT(CRITICAL, 
// 			"unhandled type(%s)", 
// 			sampler::to_string(address).data());
// 		return GL_CLAMP_TO_EDGE;
// 	}
// }