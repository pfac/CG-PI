#version 330

//	Blinn-Phong

in vec4 position;
in vec3 normal;

out vec3 eyeVS;
out vec3 normalVS;

uniform mat4 pvm;//	to transform points from model space to camera space
uniform mat4 vm;
uniform mat3 normalMatrix;//	to transform vectors from model space to camera space

void main() {
	vec3 pos = vec3( vm * position );
	gl_Position = pvm * position;
	normalVS = normalize( normalMatrix * normal );
	eyeVS = normalize( -pos );
}