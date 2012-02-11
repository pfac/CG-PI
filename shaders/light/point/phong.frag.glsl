#version 330

in vec3 eyeVS;
in vec3 normalVS;
in vec3 lightDirVS;

out vec3 color;

uniform vec3 diffuse;
uniform vec3 specular;

uniform float shininess;

void main ()
{
	vec3 n = normalize( normalVS );
	vec3 l = normalize( lightDirVS );
	float cos_a = dot(n,l);

	float intensity = max( cos_a , 0.0 );

	vec3 eye = normalize( eyeVS );
	vec3 h = normalize( l + eye );
	float s = pow( max( dot(h,n) , 0.0 ) , shininess );

	color = ( intensity + 0.33 ) * diffuse + specular * s;
}