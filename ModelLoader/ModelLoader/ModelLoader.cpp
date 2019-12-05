#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <regex>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "LoadShaders.h"
#include "controller.h"
#include "OBJLoader.h"
#include "DAELoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#include <cstdlib>
using namespace std;

//stb_image is a 3rd party library

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//Code based off: https://www.daniweb.com/programming/software-development/threads/159753/opening-a-filedialog
string getFileName(wchar_t* filter = L"All Files (*.*)\0*.*\0") {
	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = L"";


	if (GetOpenFileName(&ofn)) {
		wstring temp(fileName);
		string fileNameStr(temp.begin(), temp.end());
		return fileNameStr;
	}

	return "";
}

class model {
	GLuint program;
	GLuint modelVertexBuffer;
	GLuint modelUVBuffer;
	GLuint modelNormBuffer;
	GLuint texture;
	GLuint texID;
	GLuint MVPMatrixID;
	GLuint modelMatrixID;

	vector<glm::vec2> modelUVs;
	vector<glm::vec3> modelNormals;

	glm::mat4 modelMatrix;

	bool textureApplied = false;

public:
	GLuint VertexArrayID;
	string modelId;
	vector<glm::vec3> modelVertices;

	bool loadModel(GLuint paramProgram, unsigned int numModelsLoaded, string objectPath) {
		if (objectPath == "") {
			cout << "No file selected, returning to menu.";
			return false;
		}

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		program = paramProgram;


		bool result;

		cout << "\nFiletype? = " << objectPath[objectPath.length() - 3] << objectPath[objectPath.length() - 2] << objectPath[objectPath.length() - 1] << "\n";

		if ((objectPath[objectPath.length() - 3] == 'd' || objectPath[objectPath.length() - 3] == 'D') &&
			(objectPath[objectPath.length() - 2] == 'a' || objectPath[objectPath.length() - 2] == 'A') &&
			(objectPath[objectPath.length() - 1] == 'e' || objectPath[objectPath.length() - 1] == 'E')) {
			result = loadDAEFile(objectPath.c_str(), modelVertices, modelUVs, modelNormals, modelId);
		}
		else if ((objectPath[objectPath.length() - 3] == 'o' || objectPath[objectPath.length() - 3] == 'O') &&
			(objectPath[objectPath.length() - 2] == 'b' || objectPath[objectPath.length() - 2] == 'B') &&
			(objectPath[objectPath.length() - 1] == 'j' || objectPath[objectPath.length() - 1] == 'J')) {
			result = loadOBJFile(objectPath.c_str(), modelVertices, modelUVs, modelNormals, modelId);
		}
		else {
			cout << "\nFile type not recognised, please try again with a .obj file or a .dae file.\n";
			result = false;
		}

		if (result == false) {
			return false;
		}

		if (modelId == "unknown") {
			modelId = "Model"; 
		}

		modelId = modelId + "_" + to_string(numModelsLoaded + 1);

		modelVertexBuffer;
		glGenBuffers(1, &modelVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(glm::vec3), &modelVertices[0], GL_STATIC_DRAW);
		modelUVBuffer;
		glGenBuffers(1, &modelUVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelUVBuffer);
		glBufferData(GL_ARRAY_BUFFER, modelUVs.size() * sizeof(glm::vec2), &modelUVs[0], GL_STATIC_DRAW);
		modelNormBuffer;
		glGenBuffers(1, &modelNormBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelNormBuffer);
		glBufferData(GL_ARRAY_BUFFER, modelNormals.size() * sizeof(glm::vec3), &modelNormals[0], GL_STATIC_DRAW);
		
		modelMatrix = glm::mat4(1.0f);

		texID = glGetUniformLocation(program, "texSampler");
		MVPMatrixID = glGetUniformLocation(program, "mvp");
		modelMatrixID = glGetUniformLocation(program, "modelSpace");

		changeTexture();
		cout << "\nSuccessfully loaded model. Model's ID is: " << modelId << ". Returning to menu.\n\n";
		glBindVertexArray(NULL);
		return true;
	}

	void changeTexture() { //Tex load code uses code provided in SOFT356 repo
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLint width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		bool goodTextureLoaded = false;

		cout << "\nPlease select a texture file to load. Note: only 1 texture is supported per model at this time.";

		while (!goodTextureLoaded) {
			string filePath = getFileName();
			if (filePath == "") {
				cout << "\nTexture selection cancelled.";
				if (!textureApplied) {
					cout << "\nNo texture has been applied yet. Applying default texture.";
					filePath = "../Test Files (Model Loader)/texTest.png";
				}
				else {
					cout << "Returning to menu.\n\n";
					return;
				}
			}


			unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				goodTextureLoaded = true;
				textureApplied = true;
			}
			else
			{
				cout << "\nFailed to load texture, please try again.";
			}
			stbi_image_free(data);
		}

		
	}

	void modifyModelMatrix() {
		cout << "\nEntering translate\n";
		//glm::mat4 newModelMatrix = modelMatrix;		

		std::cmatch match;
		std::regex ex("(-)?(\[0-9]+)(\.\[0-9]*)?");

		std::cout << "How much should the model's 'x' be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.";

		std::string input;
		float xChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			xChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "How much should the model's 'y' be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.";

		float yChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			yChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "How much should the model's 'z' be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.";

		float zChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			zChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "\n";

		modelMatrix = glm::translate(modelMatrix, glm::vec3(xChange, yChange, zChange));

		//modelMatrix = newModelMatrix;
		cout << "Translation applied, returning to menu.";
	}

	void modifyModelScale() {
		cout << "\nEntering Scale\n";
		//glm::mat4 newModelMatrix = modelMatrix;

		std::cmatch match;
		std::regex ex("(-)?(\[0-9]+)(\.\[0-9]*)?");

		std::cout << "How much should the model's 'x' be scaled by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << modelMatrix[0][0] << "\t";

		std::string input;
		float xChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			xChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "How much should the model's 'y' be scaled by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << modelMatrix[1][1] << "\t";

		float yChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			yChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "How much should the model's 'z' be scaled by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << modelMatrix[2][2] << "\t";

		float zChange = 0.0f;
		std::cin >> input;

		if (std::regex_match(input.c_str(), match, ex)) {
			zChange = stof(match[0]);
		}
		else {
			std::cout << "\nInvalid value entered, canelling.";
			return;
		}

		std::cout << "\n";

		modelMatrix[0][0] += xChange;
		modelMatrix[1][1] += yChange;
		modelMatrix[2][2] += zChange;

		//modelMatrix = newModelMatrix;
		cout << "Translation applied, returning to menu.";
	}
	
	void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
		glUseProgram(program);
		glBindVertexArray(VertexArrayID);

		//modelMatrix = glm::mat4(1.0f);
		glm::mat4 modelViewProjection = projectionMatrix * viewMatrix * modelMatrix;

		glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &modelViewProjection[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(texID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, modelVertexBuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, modelUVBuffer);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, modelNormBuffer);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(NULL);
	}

	void cleanup() {
		// Cleanup VBO and shader
		glDeleteBuffers(1, &modelVertexBuffer);
		glDeleteBuffers(1, &modelUVBuffer);
		glDeleteBuffers(1, &modelNormBuffer);
		glDeleteTextures(1, &texture);
		glDeleteVertexArrays(1, &VertexArrayID);
	}
};

class pipeline{

	GLFWwindow* win;
	GLuint winWidth;
	GLuint winHeight;
	unsigned int numModelsLoaded = 0;

	vector<model> modelList;

	bool menuOpen = false;
	bool quitProg = false;

public:
	void openMenu() {
		menuOpen = true;
		cout << "\n\nOpening menu\n";
	}

	void assignWindow(GLFWwindow* win, GLfloat width, GLfloat height) {
		this->win = win;
		this->winWidth = width;
		this->winHeight = height;
	}

	void loadNewModel(GLuint program) {
		model newModel;
		cout << "\n\nPlease select an OBJ or DAE file to load:";
		string objPath = getFileName(L"OBJ Files (*.obj)\0*.obj\0DAE Files(*.dae)\0*.dae\0");

		bool success = newModel.loadModel(program, numModelsLoaded, objPath);

		if (success) {
			modelList.push_back(newModel);
			numModelsLoaded++;
		}
		else {
			newModel.cleanup();
		}
	}

	void listModelIDs() {
		if (modelList.size() == 0) {
			cout << "\nNo models to list.\n";
			return;
		}

		cout << "\n\nListing " << modelList.size() << " Model IDs:";
		for (unsigned int i = 0; i < modelList.size(); i++) {
			cout << "\n\t" << modelList[i].modelId;
		}
		cout << "\nEnd of Model ID list.";
	}

	void clearModel() {
		if (modelList.size() == 0) {
			cout << "\nNo models to delete.\n";
			return;
		}
		cout << "\n\nPlease enter a model ID to delete from scene (case sensitive): ";
		string modelID;
		getline(cin, modelID);

		bool match = false;

		for (unsigned int i = 0; i < modelList.size(); i++) {
			if (modelList[i].modelId == modelID) {
				modelList[i].cleanup();
				modelList.erase(modelList.begin()+i);
				match = true;
			}
		}

		if (match) {
			cout << "\nModel erased from scene.\n";
		}
		else {
			cout << "\nModel not found in scene. Returning to menu.\n";
		}
	}

	void changeModelTexture() {
		if (modelList.size() == 0) {
			cout << "\nNo models to change texture on.\n";
			return;
		}
		cout << "\n\nPlease enter a model ID to modify its texture (case sensitive): ";
		string modelID;
		getline(cin, modelID);

		bool match = false;

		for (unsigned int i = 0; i < modelList.size(); i++) {
			if (modelList[i].modelId == modelID) {
				modelList[i].changeTexture();
				match = true;
			}
		}

		if (!match) {
			cout << "\nModel not found in scene. Returning to menu.\n";
		}
	}

	void translateModelMatrix() {
		if (modelList.size() == 0) {
			cout << "\nNo models to change matrix of.\n";
			return;
		}
		cout << "\n\nPlease enter a model ID to modify its position (case sensitive): ";
		string modelID;
		getline(cin, modelID);

		bool match = false;

		for (unsigned int i = 0; i < modelList.size(); i++) {
			if (modelList[i].modelId == modelID) {
				modelList[i].modifyModelMatrix();
				match = true;
			}
		}

		if (!match) {
			cout << "\nModel not found in scene. Returning to menu.\n";
		}
	}

	void scaleModelMatrix() {
		if (modelList.size() == 0) {
			cout << "\nNo models to change matrix of.\n";
			return;
		}
		cout << "\n\nPlease enter a model ID to modify its scale (case sensitive): ";
		string modelID;
		getline(cin, modelID);

		bool match = false;

		for (unsigned int i = 0; i < modelList.size(); i++) {
			if (modelList[i].modelId == modelID) {
				modelList[i].modifyModelScale();
				match = true;
			}
		}

		if (!match) {
			cout << "\nModel not found in scene. Returning to menu.\n";
		}
	}

	void initGL() {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		numModelsLoaded = 0;

		ShaderInfo shaders[] =
		{
			{ GL_VERTEX_SHADER, "Shaders/modelLoader.vert" },
			{ GL_FRAGMENT_SHADER, "Shaders/modelLoader.frag" },
			{ GL_NONE, NULL }
		};

		GLuint program = LoadShaders(shaders);

		GLuint viewMatrixID = glGetUniformLocation(program, "viewSpace");
		GLuint lightPosId = glGetUniformLocation(program, "lightPos");
		GLuint lightColId = glGetUniformLocation(program, "lightCol");
		GLuint lightStrId = glGetUniformLocation(program, "lightStr");

		setControllerWindow(win);
		glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_FALSE);
		glfwSetKeyCallback(win, key_callback);
		glfwSetCursorPos(win, winWidth / 2, winHeight / 2);

		glClearColor(0.6f, 0.6f, 0.8f, 0.0f);

		do {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(program);

			if (!menuOpen) {
				computeMatricesFromInputs();
			}
			else {
				cout << "\n\nMenu opened. Type a command and press enter to continue.";
				cout << "\n\tType 'quit' to quit";
				cout << "\n\tType 'load new model' to load a new model into the scene";
				cout << "\n\tType 'list existing models' to list the IDs of all models in the scene";
				cout << "\n\tType 'clear existing model' to delete an existing model from the scene";
				cout << "\n\tType 'modify model texture' to load a new texture for an existing model";
				cout << "\n\tType 'translate model' to move an existing model around the scene";
				cout << "\n\tType 'scale model' to change an existing model's size";
				cout << "\n\tType 'change horizontal sensitivity' to change horizontal mouse sensitivity";
				cout << "\n\tType 'change vertical sensitivity' to change vertical mouse sensitivity";
				cout << "\n\tType 'change movement speed' to change movement speed";
				cout << "\n\tType 'reset camera' to reset camera sensitivity and speed";
				cout << "\n\tType anything else to resume\n\n";

				string input;
				getline(cin, input);
				//Switch cant be used with strings so old fashioned way:

				if (input == "quit") {
					quitProg = true;
					continue;
				} else if(input == "change horizontal sensitivity") {
					changeHorizMouseSpeed();
				}
				else if(input == "change vertical sensitivity") {
					changeVertMouseSpeed();
				}
				else if(input == "change movement speed") {
					changeSpeed();
				}
				else if (input == "reset camera") {
					resetCamera();
				}
				else if (input == "load new model") {
					loadNewModel(program);
				}
				else if (input == "list existing models") {
					listModelIDs();
				}
				else if (input == "clear existing model") {
					clearModel();
				}
				else if (input == "modify model texture") {
					changeModelTexture();
				}
				else if (input == "translate model") {
					translateModelMatrix();
				}
				else if (input == "scale model") {
					scaleModelMatrix();
				}
				else {
					cout << "\n\nClosing menu\n";
					menuOpen = false;
				}

				continue;
			}

			glm::mat4 projectionMatrix = getProjectionMatrix();
			glm::mat4 viewMatrix = getViewMatrix();
			glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

			glUniform3f(lightPosId, 4.0f, 4.0f, 4.0f);
			glUniform3f(lightColId, 1.0f, 1.0f, 1.0f);
			glUniform1f(lightStrId, 50.0f);

			for (unsigned int i = 0; i < modelList.size(); i++) {
				modelList[i].draw(projectionMatrix, viewMatrix);
			}

			// Swap buffers
			glfwSwapBuffers(win);
			glfwPollEvents();
		} while (quitProg == false &&
			glfwWindowShouldClose(win) == 0);

		glfwDestroyWindow(win);
		glDeleteProgram(program);

		for (unsigned int i = 0; i < modelList.size(); i++) {
			modelList[i].cleanup();
		}

		glfwTerminate();
	}
};

//glm::mat4 multiplyModelMatrix(glm::mat4 translationMat, glm::mat4 rotationMat, glm::mat4 scaleMat) {
	//glm::mat4 modelMat = translationMat * rotationMat * scaleMat;
	//return modelMat;
//}

//glm::mat4 CameraMatrix = glm::lookAt(cameraWorldPosition, cameraTargetWorldPosition Vector3.up);

void viewTutorial() {
	cout << "Load the first model by pressing the escape key and typing \"load new model\" into the console.\n";
	cout << "Once the model is loaded, keep this console window in view, as menu information will be printed here.\n";
	cout << "Use the WASD keys to move and strafe the camera, QE to raise/lower the camera respectively, and the mouse to rotate the camera.\n";
	cout << "Hold the Shift key to make the camera move faster.\n";
	cout << "The camera will likely initially be inside the model when a scene is loaded, if you cannot see the model please try and reorient yourself with the WASD keys and the mouse.\n";
	cout << "At any point press the escape key to open a list of commands.\n";
	cout << "\nThe menu will have the following commands available:\n";
	cout << "\t- Add New Model to Scene\n";
	cout << "\t- Remove Model From Scene\n";
	cout << "\t- Change Texture for Model\n";
	cout << "\t- Change Movement Speed\n";
	cout << "\t- Change Mouse Horizontal Sensitivity\n";
	cout << "\t- Change Mouse Vertical Sensitivity\n";
	cout << "\t- Reset Camera to Default\n";
	cout << "\t- Quit Program\n";
	cout << "\nEach menu option will describe what they do as well as which inputs are required.\n";
	cout << "Please note, each model that is loaded into a scene will be given a number, depending on what order they are loaded in (starting from 1).\n";
	cout << "This number will be printed to the console alongside the Model's filename for reference.\n";
	cout << "Please take note of which number is assigned to which model if you wish to use the 'Remove Model' or 'Change Texture' options.\n";
	cout << "Please quit the program by pressing 'Quit Program' or by clicking the 'X' on the Model Viewer window.\n";
	system("Pause");
}

pipeline p;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		try{
			p.openMenu();
		}
		catch (...) {}
	}
}

int main()
{
	GLuint skipTut = -1;
	cout << "Welcome to the model loader.\n";	
	cout << "Please type all menu commands in lower case.\n";
	cout << "Would you like to see a tutorial of what this loader can do? y/n\n";
	do {
		string input;
		cin >> input;
		cout << '\n';
		if (input == "y" || input == "Y") {
			skipTut = 0;
		}
		else if (input == "n" || input == "N") {
			skipTut = 1;
		}
		else {
			cout << "Please input only 'y' or 'n', then press Enter.\n";
		}
	} while (skipTut == -1);

	if (skipTut == 0) {
		viewTutorial();
	}

	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		cout << "Failed to initialize GLFW. Exiting Model Loader.";
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Floats for aspect ratio
	GLfloat windowWidth = 1280;
	GLfloat windowHeight = 720;

	GLFWwindow* win = glfwCreateWindow(windowWidth, windowHeight, "Model Loader", NULL, NULL);

	if (win == NULL) {
		//You lose. That's how that works, right?
		cout << "Failed to open a GLFW window. Exiting Model Loader.\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(win);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialise GLEW. Exiting Model Loader.\n";
		return -1;
	}
	p.assignWindow(win, windowWidth, windowHeight);
	p.initGL();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
