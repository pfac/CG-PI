#version 330

//	Blinn-Phong

in vec4 position;
in vec3 normal;

out vec3 eyeVS;
out vec3 normalVS;
out vec3 lightDirVS;

uniform mat4 pvm;//	to transform points from model space to camera space
uniform mat4 vm;//	change space: model -> world
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;//	to transform vectors from model space to camera space

uniform vec4 lightPosition;

void main() {
	vec3 pos = vec3( vm * position );
	vec3 lpos = vec3( viewMatrix * lightPosition );

	eyeVS = normalize( -pos );
	normalVS = normalize( normalMatrix * normal );
	lightDirVS = normalize( lpos - pos );

	gl_Position = pvm * position;
}