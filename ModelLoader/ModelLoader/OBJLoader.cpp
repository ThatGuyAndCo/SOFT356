#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

using namespace std;

bool loadOBJFile(const char* path, vector<glm::vec3>& ret_verts, vector<glm::vec2>& ret_uvs, vector<glm::vec3>& ret_norms, string& modelId) {
	vector<unsigned int> vertIds;
	vector<unsigned int> uvIds;
	vector<unsigned int> normIds;

	vector<glm::vec3> verts;
	vector<glm::vec2> uvs;
	vector<glm::vec3> norms;

	FILE* objFile;
	errno_t fileError;

	fileError = fopen_s(&objFile, path, "r");

	if (fileError != 0) {
		cout << "Error opening the file, file may be corrupt.";
		cout << fileError;
		return false;
	}
	else {
		try {
			const char* filePath = path;
			char readPath[1000];

			if(errno_t splitPath = _splitpath_s(
				filePath,
				NULL,
				0,
				NULL,
				0,
				readPath,
				sizeof(readPath),
				NULL,
				0
			) == 0){
				for (unsigned int i = 0; i < 999; i++) { //ignore first char as this will be a space
					if (readPath[i] == NULL)
						break;
					else {
						modelId = modelId + readPath[i];
					}
				}
			}
			else {
				cout << "Failed to extract filename, setting to unknown";
				modelId = "unknown";
			}

			cout << "\nLoading model, please wait...";

			while (true) {
				char lineHead[128];
				int line = fscanf_s(objFile, "%s", lineHead, sizeof(lineHead));

				if (line == EOF) {
					break;
				}

				if (strcmp(lineHead, "v") == 0) {
					glm::vec3 vertex;
					fscanf_s(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					//cout << "\nVertex x: " << vertex.x << ", y: " << vertex.y << ", z: " << vertex.z << "\n";
					verts.push_back(vertex);
				}
				else if (strcmp(lineHead, "vt") == 0) {
					glm::vec2 uv;
					fscanf_s(objFile, "%f %f\n", &uv.x, &uv.y);
					//cout << "\nUV u: " << uv.x << ", v: " << uv.y << "\n";
					uvs.push_back(uv);
				}
				else if (strcmp(lineHead, "vn") == 0) {
					glm::vec3 normal;
					fscanf_s(objFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					norms.push_back(normal);
				}
				else if (strcmp(lineHead, "f") == 0) {
					bool fourIndexes = false;
					unsigned int vertIndex[4];
					unsigned int uvIndex[4];
					unsigned int normalIndex[4];

					string lineAsStr = "";
					string* strPoint = &lineAsStr;

					char readChar[1000];

					fscanf_s(
						objFile, "%[^\n]",
						readChar, sizeof(readChar)
					);

					for (unsigned int i = 1; i < 999; i++) { //ignore first char as this will be a space
						if (readChar[i] == NULL)
							break;
						else{
							//cout << readChar[i];
							lineAsStr = lineAsStr + readChar[i];
						}
					}

					/*cout << "\n\nPrint line:\n";
					cout << lineAsStr;
					cout << "\nEnd line.\n\n";*/

					const char* stringArr = lineAsStr.c_str();

					vector<string> brokenStr;

					unsigned int startPoint = 0;
					unsigned int breakPoint = 0;
					for (breakPoint = 0; breakPoint < lineAsStr.length(); breakPoint++) {
						if (stringArr[breakPoint] == ' ') {
							brokenStr.push_back(lineAsStr.substr(startPoint, breakPoint - startPoint));
							startPoint = breakPoint + 1;
						}
						else if (breakPoint == lineAsStr.length() - 1) {
							brokenStr.push_back(lineAsStr.substr(startPoint, (breakPoint - startPoint) + 1));
						}
					}

					/*cout << "\n\nBroken string size:\n";
					cout << brokenStr.size();

					cout << "\n\nBroken string content:\n";
					for (unsigned int i = 0; i < brokenStr.size(); i++) {
						cout << "i = " << i << "\n";
						cout << brokenStr[i];
						cout << "\n";
					}

					cout << "\n";*/

					if (brokenStr.size() != 3 && brokenStr.size() != 4) {
						cout << "File too complex for loader. Please use a simpler model.";
						fclose(objFile);
						return false;
					}

					for (unsigned int i = 0; i < brokenStr.size(); i++) {
						string subS = brokenStr[i];
						const char* subArr = brokenStr[i].c_str();

						vector<string> f_indicies;
						unsigned int startPoint = 0;
						unsigned int breakPoint = 0;
						unsigned int indexCount = 0;


						//cout << "\n\subS = " << subS << "\n";


						for (breakPoint = 0; breakPoint < subS.length(); breakPoint++) {
							if (subArr[breakPoint] == '/') {
								f_indicies.push_back(subS.substr(startPoint, breakPoint - startPoint));
								startPoint = breakPoint + 1;
							}
							else if (breakPoint == subS.length() - 1) {
								f_indicies.push_back(subS.substr(startPoint, (breakPoint - startPoint) + 1));
							}
						}

						/*cout << "\n\nf_indicies size:\n";
						cout << f_indicies.size();

						cout << "\n\nf_indicies content:\n";
						for (unsigned int i = 0; i < f_indicies.size(); i++) {
							cout << "i = " << i << "\n";
							cout << f_indicies[i];
							cout << "\n";
						}*/

						vertIndex[i] = stoi(f_indicies[0]);
						uvIndex[i] = stoi(f_indicies[1]);
						normalIndex[i] = stoi(f_indicies[2]);


						//cout << "\n";
					}

					vertIds.push_back(vertIndex[0]);
					vertIds.push_back(vertIndex[1]);
					vertIds.push_back(vertIndex[2]);

					uvIds.push_back(uvIndex[0]);
					uvIds.push_back(uvIndex[1]);
					uvIds.push_back(uvIndex[2]);

					normIds.push_back(normalIndex[0]);
					normIds.push_back(normalIndex[1]);
					normIds.push_back(normalIndex[2]);

					if (brokenStr.size() == 4) {
						vertIds.push_back(vertIndex[0]);
						vertIds.push_back(vertIndex[2]);
						vertIds.push_back(vertIndex[3]);

						uvIds.push_back(uvIndex[0]);
						uvIds.push_back(uvIndex[2]);
						uvIds.push_back(uvIndex[3]);

						normIds.push_back(normalIndex[0]);
						normIds.push_back(normalIndex[2]);
						normIds.push_back(normalIndex[3]);
					}

				}
				else if (strcmp(lineHead, "mtl") == 0) {

				}
				else { //Dont care about line if not a vert, uv, normal or index
					char munch[1000];
					fscanf_s(objFile, ".+\n", munch, sizeof(munch));
				}
			}

			for (unsigned int i = 0; i < vertIds.size(); i++) {
				/*cout << "\ni = " << i;
				cout << ", VertId = " << vertIds[i] << ", VertValue = " << verts[vertIds[i] - 1].x << ", " << verts[vertIds[i] - 1].y << ", " << verts[vertIds[i] - 1].z << "\n";
				cout << ", UvId = " << uvIds[i] << ", UvValue = " << uvs[uvIds[i] - 1].x << ", " << uvs[uvIds[i] - 1].y << "\n";*/

				glm::vec3 vert = verts[vertIds[i] - 1];
				glm::vec2 uv = uvs[uvIds[i] - 1];
				glm::vec3 normal = norms[normIds[i] - 1];

				ret_verts.push_back(vert);
				ret_uvs.push_back(uv);
				ret_norms.push_back(normal);
			}

			fclose(objFile);
			return true;
		}
		catch (exception & e) {
			cout << "Error in reading file data. Please ensure the file is a valid .obj file. Error is as follows:";
			cout << e.what();
			fclose(objFile);
			return false;
		}
		catch (...) {
			cout << "Unknown error in reading file data. Please ensure the file is a valid .obj file.";
			fclose(objFile);
			return false;
		}
	}
}