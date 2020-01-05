#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <regex>

GLFWwindow* win;
bool menuOpen = false;

glm::vec3 pos = glm::vec3(0,1,0);
float horizAngle = 0.0f;
float vertAngle = -0.75f;
float FOV = 70.0f;

float speed = 3.0f;
float horizMouseSpeed = 0.005f;
float vertMouseSpeed = 0.005f;

float nearClippingPlane = 0.1f;
float farClippingPlane = 1000.0f;

//View Type 0 for perspective, 1 for orthogonal
int viewType = 0;

glm::mat4 vMat;
glm::mat4 pMat;

void setControllerWindow(GLFWwindow* winArg) {
	win = winArg;
}

void setMenuOpen(bool isMenuOpen) {
	menuOpen = false;
}

void computeMatricesFromInputs() {
	static double lastFrameTime = glfwGetTime();

	double currFrameTime = glfwGetTime();
	float deltaTime = float(currFrameTime - lastFrameTime);

	double xpos;
	double ypos;

	glfwGetCursorPos(win, &xpos, &ypos);

	int winWidth;
	int winHeight;
	glfwGetWindowSize(win, &winWidth, &winHeight);
	glfwSetCursorPos(win, winWidth/2, winHeight/2);

	horizAngle += horizMouseSpeed * float(winWidth / 2 - xpos); //Full rotation = 2*PI
	vertAngle += vertMouseSpeed * float(winHeight / 2 - ypos);

	//Ensure no number overflowing from constant rotation
	if (horizAngle > 3.14f)
		horizAngle = -3.14f;
	else if (horizAngle < -3.14f)
		horizAngle = 3.14f;

	//Ensure camera cannot turn upside down
	if (vertAngle > 1.5f)
		vertAngle = 1.5f;
	else if (vertAngle < -1.5f)
		vertAngle = -1.5f;

	glm::vec3 dir( //Transform angles to represent the camera forward
		cos(vertAngle) * sin(horizAngle),
		sin(vertAngle),
		cos(vertAngle) * cos(horizAngle)
	);

	glm::vec3 right( //Transform horiz angle to represent the camera right
		sin(horizAngle - 3.14f/2.0f),
		0.0f,
		cos(horizAngle - 3.14f/2.0f)
	);

	glm::vec3 up = glm::cross(right, dir); //Get cross product of right and forward to represent camera's up

	float modSpeed = speed;
	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT)) {
		modSpeed *= 1.5f;
	}

	// Move forward
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
		pos += dir * deltaTime * modSpeed;
	}
	// Move backward
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
		pos -= dir * deltaTime * modSpeed;
	}
	// Strafe right
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
		pos += right * deltaTime * modSpeed;
	}
	// Strafe left
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
		pos -= right * deltaTime * modSpeed;
	}
	// Raise camera
	if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) {
		pos += up * deltaTime * modSpeed;
	}
	// Lower camera
	if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) {
		pos -= up * deltaTime * modSpeed;
	}

	if (viewType == 0) {
		pMat = glm::perspective(
			glm::radians(FOV),
			float(winWidth) / float(winHeight),
			nearClippingPlane,
			farClippingPlane
		);
	}
	else {

	}

	vMat = glm::lookAt(
		pos,
		pos + dir,
		up
	);

	lastFrameTime = currFrameTime;
}

void centerViewMatrix() {
	std::cout << "\ncentering view pos\n";
	pos = glm::vec3(0, 1, 0);
}

glm::vec3 getViewPos() {
	return pos;
}

glm::mat4 getViewMatrix() {
	return vMat;
}

glm::mat4 getProjectionMatrix() {
	return pMat;
}

/////////////Modify Controller below/////////////

void changeSpeed() {
	std::cout << "How much should the movement speed be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << speed << "\t";

	std::string input;
	float speedChange = 0.0f;
	std::cin >> input;

	std::cmatch match;

	std::regex ex("(-)?(\[0-9]+)(\.\[0-9]*)?");
	if (std::regex_match(input.c_str(), match, ex)) {
		speedChange = stof(match[0]);
		speed += speedChange;
		if (speed < 0.1f) {
			speed = 0.1f;
			std::cout << "\nSpeed cannot go below 0.1";
		}
		std::cout << "\nSpeed is now: " << speed;
	}
	else {
		std::cout << "\nInvalid value entered, canelling.";
	}

	std::cout << "\n";
}

void changeHorizMouseSpeed() {
	std::cout << "How much should the mouse's horizontal sensitivity be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << horizMouseSpeed << "\t";

	std::string input;
	float mouseSpeedChange = 0.0f;
	std::cin >> input;


	std::cmatch match;

	std::regex ex("(-)?(\[0-9]+)(\.\[0-9]*)?");
	if (std::regex_match(input.c_str(), match, ex)) {
		mouseSpeedChange = stof(match[0]);
		horizMouseSpeed += mouseSpeedChange;
		if (horizMouseSpeed < 0.1f) {
			horizMouseSpeed = 0.1f;
			std::cout << "\nSensitivity cannot go below 0.1";
		}
		std::cout << "\nSensitivity is now: " << horizMouseSpeed;
	}
	else {
		std::cout << "\nInvalid value entered, canelling.";
	}

	std::cout << "\n";
}

void changeVertMouseSpeed() {
	std::cout << "How much should the mouse's vertical sensitivity be increased by?\nEnter a positive/negative float to continue, or any other value to cancel, then press enter.\nCurrent value is: " << vertMouseSpeed << "\t";

	std::string input;
	float mouseSpeedChange = 0.0f;
	std::cin >> input;



	std::cmatch match;

	std::regex ex("(-)?(\[0-9]+)(\.\[0-9]*)?");
	if (std::regex_match(input.c_str(), match, ex)) {
		mouseSpeedChange = stof(match[0]);
		vertMouseSpeed += mouseSpeedChange;
		if (vertMouseSpeed < 0.1f) {
			vertMouseSpeed = 0.1f;
			std::cout << "\nSensitivity cannot go below 0.1";
		}
		std::cout << "\nSensitivity is now: " << vertMouseSpeed;
	}
	else {
		std::cout << "\nInvalid value entered, canelling.";
	}

	std::cout << "\n";
}

void changeFOV() {
	float newFOV = 0.0f;

	FOV = newFOV;
}

void changeViewType() {
	int newViewType = 0;

	viewType = newViewType;
}

void resetCamera() {
	speed = 3.0f;
	horizMouseSpeed = 0.01f;
	vertMouseSpeed = 0.0075f;
	horizAngle = 0.0f;
	vertAngle = 0.0f;
	FOV = 70.0f;
}
