#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 vUV;
layout( location = 2 ) in vec3 vNorm;

uniform mat4 mvp;
uniform mat4 modelSpace;
uniform mat4 viewSpace;
uniform vec3 lightPos;

out vec2 UV;
out vec3 worldPos;
out vec3 normalFromView;
out vec3 viewDir;
out vec3 lightDirFromView;

void main()
{
	gl_Position = mvp * vec4(vPosition, 1);
	worldPos = (modelSpace * vec4(vPosition, 1)).xyz;
	vec3 vertInView = (viewSpace * modelSpace * vec4(vPosition, 1)).xyz;

	viewDir = vec3(0,0,0) - vertInView;

	vec3 lightToCamera = (viewSpace * vec4(lightPos, 1)).xyz;
	lightDirFromView = lightToCamera + viewDir;

	normalFromView = (viewSpace * modelSpace * vec4(vNorm, 0)).xyz;

	UV = vUV;
}