// Vertex shader -- directly copied from default.
#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4  position;
in vec2  texcoord;
in vec4  color;
in vec3  normal;
 
out vec2 texCoordVarying;
out vec4 normalVarying;

void main()
{
	// Transform model position to camera space.
	gl_Position = modelViewProjectionMatrix * position;

	// Transform texture coordinates.
	texCoordVarying = (textureMatrix * vec4(texcoord.x, texcoord.y, 0, 1)).xy;	

	// Transform model normal to camera space.
	normalVarying = normalize(modelViewProjectionMatrix * vec4(normal.x, normal.y, normal.z, 0));
}