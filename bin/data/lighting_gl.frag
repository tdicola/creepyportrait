// Bump mapping based on algorithm at http://www.ozone3d.net/tutorials/bump_mapping.php
// Diffuse and specular lighting from algorithms at http://www.arcsynthesis.org/gltut/
#version 150

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D ambientTex;

in vec2 texCoordVarying;
in vec4 normalVarying;
in vec4 lightDirection;
in vec4 cameraDirection;

out vec4 fragColor;

vec4 lightAmbient = vec4(0.075, 0.0, 0.0, 1.0); 
vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);
float materialShininess = 0.4;

void main(){
	vec4 materialDiffuse = texture(diffuseTex, texCoordVarying);
	vec4 materialAmbient = texture(ambientTex, texCoordVarying);
	vec4 materialSpecular = texture(specularTex, texCoordVarying);

	float lambert = clamp(dot(normalVarying, lightDirection), 0.0, 1.0);

	// For reference, blinn shading model below if gaussian is too slow.
	//vec3 halfAngle = normalize(lightDirection.xyz + cameraDirection.xyz);
	//float blinn = clamp(dot(normalVarying.xyz, halfAngle), 0.0, 1.0);
	//blinn = lambert != 0.0 ? blinn : 0.0;
	//blinn = pow(blinn, materialShininess);

	vec3 halfAngle = normalize(lightDirection.xyz + cameraDirection.xyz);
	float angleNormalHalf = acos(dot(halfAngle, normalVarying.xyz));
	float exponent = angleNormalHalf / materialShininess;
	exponent = -(exponent * exponent);
	float gaussian = exp(exponent);
	gaussian = lambert != 0.0 ? gaussian : 0.0;

	fragColor = lightAmbient * materialAmbient +
				lightDiffuse * materialDiffuse * lambert +
				lightSpecular * materialSpecular * gaussian;
}