#version 120

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
uniform sampler2D ambientTex;

varying in vec2 vTexCoord;
varying in vec4 vNormal;

vec4 light1Direction = normalize(vec4(-1, 1, -1, 0));
vec4 light1Diffuse = vec4(1, 1, 1, 1);
vec4 light1Specular = vec4(1, 1, 1, 1);

vec4 lightAmbient = vec4(0.05, 0, 0, 1); // Add subtle red ambient light to fake radiance from red curtain

void main (void)
{
	vec4 materialDiffuse = texture2D(diffuseTex, vTexCoord);
	vec4 materialSpecular = texture2D(specularTex, vTexCoord);

    float lambert = clamp(dot(vNormal, light1Direction), 0, 1);

    //vec4 reflected = reflect(-light1Direction, vNormal);
    //vec4 camera = normalize(vec4(0,0,-1,0));
    //float specular = pow(clamp(dot(reflected, camera), 0, 1), 20.0);

	vec4 final = 	lightAmbient + 									// Ambient component of lighting
					light1Diffuse * materialDiffuse * lambert;  	// Diffuse component of lighting
					//light1Specular * materialSpecular * specular;	// Specular component of lighting

	
	gl_FragColor =  vec4(final[0], final[1], final[2], 1);
}