// Fragment shader -- directly copied from default.
#version 150

uniform sampler2D diffuseTex;

in vec2 texCoordVarying;
in vec4 normalVarying;

out vec4 fragColor;

// Add subtle red ambient light to fake global ullumination from red curtain.
vec4 lightAmbient = vec4(0.05, 0, 0, 1); 

// Directional light coming in from the upper left.
vec4 light1Direction = normalize(vec4(-1, 1, -1, 0));
vec4 light1Diffuse = vec4(1, 1, 1, 1);
vec4 light1Specular = vec4(1, 1, 1, 1);

void main(){
	vec4 materialDiffuse = texture(diffuseTex, texCoordVarying);

	float lambert = clamp(dot(normalVarying, light1Direction), 0, 1);

	fragColor = lightAmbient +
				light1Diffuse * materialDiffuse * lambert;
}