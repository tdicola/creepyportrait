#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 lightCameraPosition;

in vec4 position;
in vec2 texcoord;
in vec4 color;
in vec3 normal;
in vec4 tangent;
 
out vec2 texCoordVarying;
out vec3 lightDirection;
out vec3 cameraDirection;

void main()
{
	// Transform model position to camera space.
	gl_Position = modelViewProjectionMatrix * position;

	// Transform texture coordinates.
	texCoordVarying = (textureMatrix * vec4(texcoord.x, texcoord.y, 0.0, 1.0)).xy;	

	// Transform model normal to camera space.
	vec3 normalVarying = normalize(modelViewMatrix * vec4(normal.x, normal.y, normal.z, 0)).xyz;

	// Determine light angle
	vec4 cameraPosition = modelViewMatrix * position;
	lightDirection = normalize(lightCameraPosition - cameraPosition).xyz;
	cameraDirection = normalize(-cameraPosition).xyz;

	// Compute bitangent vector
	vec3 tang = normalize(modelViewMatrix * vec4(tangent.x, tangent.y, tangent.z, 0.0)).xyz;
	vec3 bitangent = cross(normalVarying, tang) * tangent.w;

	// Transform light and camera direction to tangent space 
	lightDirection.x = dot(lightDirection, tang);
	lightDirection.y = dot(lightDirection, bitangent);
	lightDirection.z = dot(lightDirection, normalVarying);
	cameraDirection.x = dot(cameraDirection, tang);
	cameraDirection.y = dot(cameraDirection, bitangent);
	cameraDirection.z = dot(cameraDirection, normalVarying);
}