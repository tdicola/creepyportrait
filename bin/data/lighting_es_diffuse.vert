precision lowp float;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
attribute vec2 texcoord;

varying vec2 texCoordVarying;
varying vec4 normalVarying;

void main(){
	// Transform model position to camera space.
	gl_Position = modelViewProjectionMatrix * position;

	// Transform texture coordinates.
	texCoordVarying = (textureMatrix * vec4(texcoord.x, texcoord.y, 0, 1)).xy;	

	// Transform model normal to camera space.
	normalVarying = normalize(modelViewProjectionMatrix * vec4(normal.x, normal.y, normal.z, 0));
}