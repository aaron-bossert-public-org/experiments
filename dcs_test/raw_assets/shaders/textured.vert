#version 450
#extension GL_ARB_separate_shader_objects : enable

struct model
{
	vec3 pos;
	float scale;
};

layout( set = 0, binding = 1 ) uniform raster_data
{
	mat4 view;
	mat4 proj;
}
raster;


layout( set = 2, binding = 3 ) readonly buffer instance_data
{
	mat4 models[];
}
instance;

layout( location = 0 ) in vec3 pos;
layout( location = 2 ) in vec2 uv0;

layout( location = 0 ) out vec2 fragTexCoord;

void main()
{
	vec4 world_pos = instance.models[gl_InstanceIndex] * vec4( pos, 1.0 );

	gl_Position = raster.proj * raster.view * world_pos;

	fragTexCoord = uv0;
}
