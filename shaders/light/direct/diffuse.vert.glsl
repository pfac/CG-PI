#version 330

in vec4 position;
in vec3 normal;

out vec3 colorVS;

uniform mat4 pvm;//	to transform points from model space to camera space
uniform mat3 normalMatrix;//	to transform vectors from model space to camera space

uniform vec3 light_dir;
uniform vec3 diffuse;

void main() {
	vec3 n = normalize(normalMatrix * normal);
	vec3 l = normalize(light_dir);
	float intensity = max( dot(n,l) , 0.0 );

	colorVS = intensity * diffuse;

	gl_Position = pvm * position;
}