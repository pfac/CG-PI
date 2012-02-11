#version 330

in vec3 eyeVS;
in vec3 normalVS;
in vec3 lightDirVS;

out vec3 color;

uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 spotDirection;

uniform float shininess;
uniform float spotLimitAngle;

void main ()
{
	vec3 l = normalize( lightDirVS );
	vec3 sd = normalize( - spotDirection );

	if ( acos( dot( sd , l ) ) < spotLimitAngle )
	{
		vec3 n = normalize( normalVS );

		float cos_a = dot(n,l);
		float intensity = max( cos_a , 0.0 );

		vec3 eye = normalize( eyeVS );
		vec3 h = normalize( l + eye );
		float s = pow( max( dot(h,n) , 0.0 ) , shininess );

		color = ( intensity + 0.33 ) * diffuse + specular * s;
	}
	else
		color = 0.33 * diffuse;
}