precision highp float;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 lightCameraPosition;

attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
attribute vec2 texcoord;

varying vec2 texCoordVarying;
varying vec3 normalVarying;
varying vec3 lightDirection;
varying vec3 cameraDirection;

void main()
{
	// Transform model position to camera space.
	gl_Position = modelViewProjectionMatrix * position;

	// Transform texture coordinates.
	texCoordVarying = (textureMatrix * vec4(texcoord.x, texcoord.y, 0, 1)).xy;	

	// Transform model normal to camera space.
	normalVarying = normalize(modelViewMatrix * vec4(normal.x, normal.y, normal.z, 0)).xyz;

	// Determine light angle
	vec4 cameraPosition = modelViewMatrix * position;
	lightDirection = normalize(lightCameraPosition - cameraPosition).xyz;
	cameraDirection = normalize(-cameraPosition).xyz;
}