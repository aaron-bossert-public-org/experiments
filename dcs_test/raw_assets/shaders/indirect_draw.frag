#version 450

layout( location = 0 ) in vec3 inNormal;
layout( location = 1 ) in vec4 inColor;
layout( location = 2 ) in vec3 inLightVec;

layout( location = 0 ) out vec4 outFragColor;

void main()
{
	outFragColor = vec4( 1, 1, 1, 1 );
	vec3 N = normalize( inNormal );
	vec3 L = normalize( inLightVec );
	vec3 ambient = vec3( 0.5 );
	vec3 diffuse = vec3( max( dot( N, L ), -0.75 ) * 0.75 );
	outFragColor = vec4( ( ambient + diffuse ) * inColor.rgb, 1.0 );
}
