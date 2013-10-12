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
attribute vec4 tangent;

varying vec2 texCoordVarying;
varying vec3 lightDirection;
varying vec3 cameraDirection;

void main()
{
	// Transform model position to camera space.
	gl_Position = modelViewProjectionMatrix * position;

	// Transform texture coordinates.
	texCoordVarying = (textureMatrix * vec4(texcoord.x, texcoord.y, 0, 1)).xy;	

	// Transform model normal to camera space.
	vec3 normalVarying = normalize(modelViewMatrix * vec4(normal.x, normal.y, normal.z, 0)).xyz;

	// Determine light angle
	vec4 cameraPosition = modelViewMatrix * position;
	lightDirection = normalize(lightCameraPosition - cameraPosition).xyz;
	cameraDirection = normalize(-cameraPosition).xyz;

	// Compute bitangent vector
	vec3 tang = normalize(modelViewMatrix * vec4(tangent.x, tangent.y, tangent.z, 0)).xyz;
	vec3 bitangent = cross(normalVarying, tang) * tangent.w;

	// Transform light and camera direction to tangent space 
	lightDirection.x = dot(lightDirection, tang);
	lightDirection.y = dot(lightDirection, bitangent);
	lightDirection.z = dot(lightDirection, normalVarying);
	cameraDirection.x = dot(cameraDirection, tang);
	cameraDirection.y = dot(cameraDirection, bitangent);
	cameraDirection.z = dot(cameraDirection, normalVarying);
}