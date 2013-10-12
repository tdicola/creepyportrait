// Diffuse and specular lighting from http://www.arcsynthesis.org/gltut/

precision highp float;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D ambientTex;

varying vec2 texCoordVarying;
varying vec3 normalVarying;
varying vec3 lightDirection;
varying vec3 cameraDirection;

vec4 lightAmbient = vec4(0.075, 0.0, 0.0, 1.0); 
vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);
float materialShininess = 0.4;

void main(){
	vec4 materialDiffuse = texture2D(diffuseTex, texCoordVarying);
	vec4 materialAmbient = texture2D(ambientTex, texCoordVarying);
	vec4 materialSpecular = texture2D(specularTex, texCoordVarying);

	float lambert = clamp(dot(normalVarying, lightDirection), 0.0, 1.0);

	vec3 halfAngle = normalize(lightDirection + cameraDirection);
	float angleNormalHalf = acos(dot(halfAngle, normalVarying));
	float exponent = angleNormalHalf / materialShininess;
	exponent = -(exponent * exponent);
	float gaussian = exp(exponent);
	gaussian = lambert != 0.0 ? gaussian : 0.0;

	gl_FragColor = lightAmbient * materialAmbient +
				   lightDiffuse * materialDiffuse * lambert +
				   //lightDiffuse * lambert;
				   lightSpecular * materialSpecular * gaussian;
}