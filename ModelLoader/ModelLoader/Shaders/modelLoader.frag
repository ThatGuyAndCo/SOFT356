#version 450 core

in vec2 UV;
in vec3 worldPos;
in vec3 normalFromView;
in vec3 viewDir;
in vec3 lightDirFromView;

uniform sampler2D texSampler;
uniform vec3 lightPos;
uniform vec3 lightCol;
uniform float lightStr;

out vec4 colour;

void main()
{
	vec3 matDiffuse = texture(texSampler, UV).rgb;
	float alpha = texture(texSampler, UV).a;
	vec3 matAmbient = vec3(0.2, 0.2, 0.2) * matDiffuse;
	vec3 matSpecular = vec3(0.3, 0.3, 0.3);

	float distToLight = length(lightPos - worldPos);
	vec3 norm = normalize(normalFromView);
	vec3 light = normalize(lightDirFromView);
	vec3 reflection = reflect(-light, norm);
	vec3 view = normalize(viewDir);

	float normToLight = clamp(dot(norm, light), 0, 1);
	float reflectToView = clamp(dot(view, reflection), 0, 1);

	colour = vec4((matDiffuse * lightCol * lightStr * normToLight / pow(distToLight, 2)) +
		matAmbient + 
		(matSpecular * lightCol * lightStr * pow(reflectToView, 3) / pow(distToLight, 2)), 
		alpha);
	//colour = texture(texSampler, UV);
}