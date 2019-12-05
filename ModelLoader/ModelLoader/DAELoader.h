#ifndef DAELOADER_H
#define DAELOADER_H

bool loadDAEFile(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::string& modelId
);

#endif