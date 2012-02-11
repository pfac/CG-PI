#version 330

//	Blinn-Phong

in vec4 position;
in vec3 normal;

out vec3 colorVS;

uniform mat4 pvm;//	to transform points from model space to camera space
uniform mat4 vm;
uniform mat3 normalMatrix;//	to transform vectors from model space to camera space

uniform vec3 light_dir;
uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 specular;

uniform float shininess;

void main() {
	vec3 n = normalize(normalMatrix * normal);
	vec3 l = normalize(light_dir);
	float cos_a = dot(n,l);
	float intensity = max( dot(n,l) , 0.0 );

	vec3 r = ( -l ) - 2 * cos_a * n;

	vec3 pos = vec3( vm * position );
	vec3 eye = normalize( -pos );
	vec3 h = normalize( l + eye );
	float s = pow( max( dot(h,n) , 0.0 ) , shininess );

	colorVS = ( intensity + 0.33 ) * diffuse + specular * s;

	gl_Position = pvm * position;
}