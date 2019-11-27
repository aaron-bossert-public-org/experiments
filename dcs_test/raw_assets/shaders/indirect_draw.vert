#version 450


layout( constant_id = 0 ) const int MAX_INSTANCE_ID = 4096;

layout( set = 0, binding = 1 ) uniform batch_data
{
	mat4 view;
	mat4 proj;
}
batch;

layout( set = 2, binding = 3 ) uniform instance_data
{
	mat4 models[MAX_INSTANCE_ID];
}
instance;

// Vertex attributes
layout( location = 0 ) in vec4 pos;
layout( location = 1 ) in vec3 nrm;
layout( location = 2 ) in vec3 cl0;

layout( location = 0 ) out vec3 outNormal;
layout( location = 1 ) out vec3 outColor;
layout( location = 3 ) out vec3 outLightVec;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	vec4 world_pos = instance.models[gl_InstanceIndex] * pos;

	gl_Position = batch.proj * batch.view * world_pos;

	vec4 lPos = vec4( 0.0, 10.0, 50.0, 1.0 );
	outLightVec = lPos.xyz - world_pos.xyz;
	outColor = cl0;
	outNormal = nrm;
}
