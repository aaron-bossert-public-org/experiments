#version 450
#extension GL_ARB_separate_shader_objects : enable

#define type readonly buffer
//#define type uniform

layout( set = 0, binding = 1 ) type batch_data
{
	mat4 view;
	mat4 proj;
}
batch;


layout( set = 2, binding = 3 ) type instance_data
{
	mat4 model;
}
instance;

layout( location = 0 ) in vec3 pos;
layout( location = 1 ) in vec3 col;
layout( location = 2 ) in vec2 uv0;

layout( location = 0 ) out vec3 fragColor;
layout( location = 1 ) out vec2 fragTexCoord;

void main()
{
	gl_Position = batch.proj * batch.view * instance.model * vec4( pos, 1.0 );

	fragColor = col;
	fragTexCoord = uv0;
}
