#version 120

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 normal;
in vec2 texcoord;

varying out vec2 vTexCoord;
//varying out vec4 interpColor;
varying out vec4 vNormal;
 
void main()
{
	vTexCoord = texcoord;
    gl_Position = modelViewProjectionMatrix * position;

    vec4 norman = vec4(normal[0], normal[1], normal[2], 0);
    vNormal = normalize(modelViewProjectionMatrix * norman);
    
    //float cosAngIncidence = dot(normCamSpace,normalize(dirToLight));
    //cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
    //interpColor = vec4(lightIntensity[0] * cosAngIncidence, lightIntensity[1] * cosAngIncidence, lightIntensity[2] * cosAngIncidence, 1);
}