#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <pugixml.hpp>

#include <glm/glm.hpp>

using namespace std;

//This loader has been created from scratch so it may be a little inefficient / clunky
//Information about how to use the pugi was obtained from here: https://stackoverflow.com/questions/16155888/proper-way-to-parse-xml-using-pugixml
//And using unofficial documentation from here: https://www.gerald-fahrnholz.eu/sw/DocGenerated/HowToUse/html/group___grp_pugi_xml_example_cpp_file.html
bool loadDAEFile(const char* path, vector<glm::vec3>& ret_verts, vector<glm::vec2>& ret_uvs, vector<glm::vec3>& ret_norms, string& modelId) {
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

			if (errno_t splitPath = _splitpath_s(
				filePath,
				NULL,
				0,
				NULL,
				0,
				readPath,
				sizeof(readPath),
				NULL,
				0
			) == 0) {
				for (unsigned int i = 0; i < (sizeof(readPath) / sizeof(readPath[0])); i++) { //ignore first char as this will be a space
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

			cout << "\nLoading DAE file, please wait...";

			// Create empty XML document within memory
			pugi::xml_document doc;
			// Load XML file into memory
			// Remark: to fully read declaration entries you have to specify
			// "pugi::parse_declaration"
			pugi::xml_parse_result result = doc.load_file(path, pugi::parse_default);
			if (!result)
			{
				std::cout << "\nParse error: " << result.description() << ", character pos= " << result.offset << "\n";
				return false;
			}
			// A valid XML document must have a single root node
			pugi::xml_node root = doc.document_element();
			if (root == NULL) {
				cout << "\nNo nodes found in document. Returning to menu.\n";
				return false;
			}

			pugi::xpath_node xpathNode = root.select_node("library_geometries/geometry/mesh"); //Should place us at the mesh node

			pugi::xml_node currentNode;
			if (xpathNode)
			{
				currentNode = xpathNode.node();
			}
			else {
				cout << "\nNo mesh found in document. DAE file may be corrupt or too complex for this simple parser. Returning to menu.\n";
				return false;
			}
			
			unsigned int indexCount = 3;
			unsigned int vertexOff = 0;
			unsigned int normalOff = 1;
			unsigned int uvOff = 2;
			for (pugi::xml_node sourceNode = currentNode.first_child(); sourceNode; sourceNode = sourceNode.next_sibling()) { //Should move through the 1st gen children of the mesh node (i.e. the source nodes and the triangles node)
				//cout << "\nI'm doing stuff at line 98!";
				string sourceNodeName(sourceNode.name());
				if (sourceNodeName == "triangles") {
					for (pugi::xml_attribute attr = sourceNode.first_attribute(); attr; attr = attr.next_attribute()) //loop through attributes of the sourceChildNode
					{
						string attribName(attr.name());
						if (attribName == "count") {
							string caster(attr.value());
							indexCount = stoi(caster) * 3 * 3; //The count here is the triangle count. We need to multiply this by 3 to get a pos, uv and norm for each triangle, and by 3 again to get that data for each vertex
						}
					}
				}

				for (pugi::xml_node sourceChildNode = sourceNode.first_child(); sourceChildNode; sourceChildNode = sourceChildNode.next_sibling()) { //Should move through the 1st gen children of the source node (i.e. float_array and technique)
					//cout << "\nI'm doing stuff at line 112!";
					//cout << sourceChildNode.name() << "\n";
					//cout << sourceChildNode.child_value() << "\n";
					string nodeName(sourceChildNode.name());
					string vertexNodeType = "";
					unsigned int vertexNodeCount;

					if (nodeName == "float_array") {
						for (pugi::xml_attribute attr = sourceChildNode.first_attribute(); attr; attr = attr.next_attribute()) //loop through attributes of the sourceChildNode
						{
							//cout << "\nI'm doing stuff at line 122!";
							string attribName(attr.name());
							if (attribName == "id") {
								string id(attr.value());
								size_t match;
								match = id.find("mesh-positions");
								if (match != string::npos) {
									vertexNodeType = "POS";
								}
								match = id.find("mesh-normals");
								if (match != string::npos) {
									vertexNodeType = "NORM";
								}
								match = id.find("mesh-map");
								if (match != string::npos) {
									vertexNodeType = "UV";
								}
							}
							if (attribName == "count") {
								string strCount(attr.value());
								vertexNodeCount = stoi(strCount);
							}
						}

						string arrayData(sourceChildNode.child_value());
						const char* stringArr = arrayData.c_str();

						vector<string> brokenStr;

						unsigned int startPoint = 0;
						unsigned int breakPoint = 0;
						for (breakPoint = 0; breakPoint < arrayData.length(); breakPoint++) {
							//cout << "\nI'm doing stuff at line 154!";
							if (stringArr[breakPoint] == ' ') {
								brokenStr.push_back(arrayData.substr(startPoint, breakPoint - startPoint));
								startPoint = breakPoint + 1;
							}
							else if (breakPoint == arrayData.length() - 1) {
								brokenStr.push_back(arrayData.substr(startPoint, (breakPoint - startPoint) + 1));
							}
						}

						if (vertexNodeType == "POS" || vertexNodeType == "NORM") {
							glm::vec3 nodeData;
							//cout << "\n\nVertex Node Type" << vertexNodeType;
							for (unsigned int i = 0; i < vertexNodeCount; i = i + 3) {
								//cout << "\nI'm doing stuff at line 167!";
								//cout << "\nVertex Node Count = " << vertexNodeCount << ", and i = " << i;
								nodeData.x = stof(brokenStr[i]);
								nodeData.y = stof(brokenStr[i + 1]);
								nodeData.z = stof(brokenStr[i + 2]);
								//cout << "Node data values: x = " << nodeData.x << ", y = " << nodeData.y << ", z = " << nodeData.z;
								
								if (vertexNodeType == "POS") {
									verts.push_back(nodeData);
								}
								else {
									norms.push_back(nodeData);
								}
							}
							cout << "\n\n";
						}
						else if (vertexNodeType == "UV") {
							glm::vec2 nodeData;
							cout << "\n\nVertex Node Type" << vertexNodeType;
							for (unsigned int i = 0; i < vertexNodeCount; i = i + 2) {
								//cout << "\nI'm doing stuff at line 183!";
								//cout << "\nVertex Node Count = " << vertexNodeCount << ", and i = " << i;
								nodeData.x = stof(brokenStr[i]);
								nodeData.y = stof(brokenStr[i + 1]);
								//cout << "Node data values: x = " << nodeData.x << ", y = " << nodeData.y;

								uvs.push_back(nodeData);
							}
							cout << "\n\n";
						}
					}
					else if (nodeName == "input") {
						string whichSemantic = "";
						for (pugi::xml_attribute attr = sourceChildNode.first_attribute(); attr; attr = attr.next_attribute()) //loop through attributes of the sourceChildNode
						{
							//cout << "\nI'm doing stuff at line 195!";
							string attribName(attr.name());
							if (attribName == "semantic") {
								string caster(attr.value());
								whichSemantic = caster;
							}
							if (attribName == "offset") {
								string caster(attr.value());
								if(whichSemantic == "VERTEX")
									vertexOff = stoi(caster);
								if (whichSemantic == "NORMAL")
									normalOff = stoi(caster);
								if (whichSemantic == "TEXCOORD")
									uvOff = stoi(caster);
							}
						}
					}
					else if (nodeName == "p") {
						string arrayData(sourceChildNode.child_value());
						//cout << "\nIndices string : " << arrayData;
						const char* stringArr = arrayData.c_str();

						vector<string> brokenStr;

						unsigned int startPoint = 0;
						unsigned int breakPoint = 0;
						for (breakPoint = 0; breakPoint < arrayData.length(); breakPoint++) {
							//cout << "\nI'm doing stuff at line 221!";
							if (stringArr[breakPoint] == ' ') {
								brokenStr.push_back(arrayData.substr(startPoint, breakPoint - startPoint));
								startPoint = breakPoint + 1;
							}
							else if (breakPoint == arrayData.length() - 1) {
								brokenStr.push_back(arrayData.substr(startPoint, (breakPoint - startPoint) + 1));
							}
						}

						glm::vec3 indices;
						//cout << "\n\nSorting out indices, Index count = " << indexCount;
						//cout << "\nVertex Offset = " << vertexOff << ", Normal Offset = " << normalOff << ", UV Offset = " << uvOff;
						for (unsigned int i = 0; i < indexCount; i = i + 3) {
							//cout << "\nI'm doing stuff at line 233!";
							indices.x = stoi(brokenStr[i]);
							indices.y = stoi(brokenStr[i + 1]);
							indices.z = stoi(brokenStr[i + 2]);

							//cout << "\nPushing following values: 0 = " << indices.x << ", 1 = " << indices.y << ", 2 = " << indices.z;

							if (vertexOff == 0)	vertIds.push_back(indices.x);
							else if (vertexOff == 1) vertIds.push_back(indices.y);
							else if (vertexOff == 2) vertIds.push_back(indices.z);

							if (normalOff == 0)	normIds.push_back(indices.x);
							else if (normalOff == 1) normIds.push_back(indices.y);
							else if (normalOff == 2) normIds.push_back(indices.z);

							if (uvOff == 0)	uvIds.push_back(indices.x);
							else if (uvOff == 1) uvIds.push_back(indices.y);
							else if (uvOff == 2) uvIds.push_back(indices.z);
						}
					}
				}
			}

			for (unsigned int i = 0; i < vertIds.size(); i++) {
				/*cout << "\nI'm doing stuff at line 255!";
				cout << "\ni = " << i;
				cout << ", VertId = " << vertIds[i] << ", VertValue = " << verts[vertIds[i]].x << ", " << verts[vertIds[i]].y << ", " << verts[vertIds[i]].z << "\n";
				cout << ", UvId = " << uvIds[i] << ", UvValue = " << uvs[uvIds[i]].x << ", " << uvs[uvIds[i]].y << "\n";
				cout << ", NormId = " << normIds[i] << ", NormValue = " << norms[normIds[i]].x << ", " << norms[normIds[i]].y << ", " << norms[normIds[i]].z << "\n";*/

				glm::vec3 vert = verts[vertIds[i]];
				glm::vec2 uv = uvs[uvIds[i]];
				glm::vec3 normal = norms[normIds[i]];

				ret_verts.push_back(vert);
				ret_uvs.push_back(uv);
				ret_norms.push_back(normal);
			}

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