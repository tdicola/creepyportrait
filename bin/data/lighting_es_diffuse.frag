precision lowp float;

uniform sampler2D diffuseTex;

varying vec2 texCoordVarying;
varying vec4 normalVarying;

// Add subtle red ambient light to fake global ullumination from red curtain.
vec4 lightAmbient = vec4(0.05, 0, 0, 1); 

// Directional light coming in from the upper left.
vec4 light1Direction = normalize(vec4(-1, 1, -1, 0));
vec4 light1Diffuse = vec4(1, 1, 1, 1);
vec4 light1Specular = vec4(1, 1, 1, 1);

void main(){
	vec4 materialDiffuse = texture2D(diffuseTex, texCoordVarying);

	float lambert = clamp(dot(normalVarying, light1Direction), 0.0, 1.0);

	gl_FragColor = lightAmbient +
				   light1Diffuse * materialDiffuse * lambert;
}