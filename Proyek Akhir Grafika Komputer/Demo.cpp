#include "Demo.h"
#include "math.h";

Demo::Demo() {

}

Demo::~Demo() {
}

void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);
	BuildTexturedCube();
	BuildTexturedPlane();
	//BuildSkyBox();
	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
}

// process all input: query GLFW whether relevant
//  keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);

}

void Demo::Update(double deltaTime) {
}

void Demo::Render() {
	/*glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)this->screenWi
	dth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::vec3 viewPos = glm::vec3(1, 1, 1);
	glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	*/

	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(56/255.0f, 235/ 255.0f, 235/ 255.0f, 0.98f);//56, 235, 235

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);



	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::vec3 viewPos = glm::vec3(1, 1, 1);
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// set lighting attribute
	GLint lightPosLoc = glGetUniformLocation(this->shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, 5, 10, 8.5);
	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
	GLint lightColorLoc = glGetUniformLocation(this->shaderProgram, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	//DrawYard();
	DrawTexturedCube();
	DrawTexturedPlane();
	//DrawSky();
	glDisable(GL_DEPTH_TEST);
}
/**/
void Demo::BuildSkyBox()
{
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("crate.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-0.5, -0.5, 0.5, 0, 0,  // 0
		0.5, -0.5, 0.5, 1, 0,   // 1
		0.5,  0.5, 0.5, 1, 1,   // 2
		-0.5,  0.5, 0.5, 0, 1,  // 3

		// right
		0.5,  0.5,  0.5, 0, 0,  // 4
		0.5,  0.5, -0.5, 1, 0,  // 5
		0.5, -0.5, -0.5, 1, 1,  // 6
		0.5, -0.5,  0.5, 0, 1,  // 7

		// back
		-0.5, -0.5, -0.5, 0, 0, // 8 
		0.5,  -0.5, -0.5, 1, 0, // 9
		0.5,   0.5, -0.5, 1, 1, // 10
		-0.5,  0.5, -0.5, 0, 1, // 11

		// left
		-0.5, -0.5, -0.5, 0, 0, // 12
		-0.5, -0.5,  0.5, 1, 0, // 13
		-0.5,  0.5,  0.5, 1, 1, // 14
		-0.5,  0.5, -0.5, 0, 1, // 15

		// upper
		0.5, 0.5,  0.5, 0, 0,   // 16
		-0.5, 0.5,  0.5, 1, 0,  // 17
		-0.5, 0.5, -0.5, 1, 1,  // 18
		0.5, 0.5, -0.5, 0, 1,   // 19

		// bottom
		-0.5, -0.5, -0.5, 0, 0, // 20
		0.5, -0.5, -0.5, 1, 0,  // 21
		0.5, -0.5,  0.5, 1, 1,  // 22
		-0.5, -0.5,  0.5, 0, 1, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::DrawSky()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, 10, -50));
	//model = glm::rotate(model,angle, glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(200, 100, 0.1));


	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildTexturedCube() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, normals
		// front
		-0.5, -0.5, 0.5,  0.0f,  0.0f,  1.0f, // 0
		0.5, -0.5, 0.5,   0.0f,  0.0f,  1.0f, // 1
		0.5,  0.5, 0.5,   0.0f,  0.0f,  1.0f, // 2
		-0.5,  0.5, 0.5,  0.0f,  0.0f,  1.0f, // 3

		// right
		0.5,  0.5,  0.5, 1.0f,  0.0f,  0.0f, // 4
		0.5,  0.5, -0.5, 1.0f,  0.0f,  0.0f, // 5
		0.5, -0.5, -0.5, 1.0f,  0.0f,  0.0f, // 6
		0.5, -0.5,  0.5, 1.0f,  0.0f,  0.0f, // 7

		// back
		-0.5, -0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 8 
		0.5,  -0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 9
		0.5,   0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 10
		-0.5,  0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-0.5, -0.5, -0.5, -1.0f,  0.0f,  0.0f, // 12
		-0.5, -0.5,  0.5, -1.0f,  0.0f,  0.0f, // 13
		-0.5,  0.5,  0.5, -1.0f,  0.0f,  0.0f, // 14
		-0.5,  0.5, -0.5, -1.0f,  0.0f,  0.0f, // 15

		// upper
		0.5, 0.5,  0.5, 0.0f,  1.0f,  0.0f, // 16
		-0.5, 0.5, 0.5, 0.0f,  1.0f,  0.0f, // 17
		-0.5, 0.5, -0.5,0.0f,  1.0f,  0.0f, // 18
		0.5, 0.5, -0.5, 0.0f,  1.0f,  0.0f, // 19

		// bottom
		-0.5, -0.5, -0.5, 0.0f,  -1.0f,  0.0f, // 20
		0.5, -0.5, -0.5,  0.0f,  -1.0f,  0.0f, // 21
		0.5, -0.5,  0.5,  0.0f,  -1.0f,  0.0f, // 22
		-0.5, -0.5,  0.5, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define normal pointer layout 2
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

static void tree(GLint objectColorLoc, GLint modelLoc, float treeX, float treeZ) {
	glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(treeX, 1, treeZ));
	model = glm::scale(model, glm::vec3(0.3, 2, 0.3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

			glm::mat4 model2;
			model2 = glm::translate(model2, glm::vec3(treeX, 2, treeZ));
			model2 = glm::scale(model2, glm::vec3(1, 1, 1));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		for (int j = 0; j < 3; j++) {
			glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

			glm::mat4 model2;
			model2 = glm::translate(model2, glm::vec3(treeX, 2, treeZ));
			if (j == 0) {
				model2 = glm::scale(model2, glm::vec3(1 + 0.1 * i, 1 - 0.1 * i, 1 - 0.1 * i));
			}
			else if (j == 1) {
				model2 = glm::scale(model2, glm::vec3(1 - 0.1 * i, 1 + 0.1 * i, 1 - 0.1 * i));
			}
			else {
				model2 = glm::scale(model2, glm::vec3(1 - 0.1 * i, 1 - 0.1 * i, 1 + 0.1 * i));
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

}

static void hedge(GLint objectColorLoc, GLint modelLoc, float hedgelocX, float hedgelocZ, int hedgecount) {
	
	for (int i = 0; i < hedgecount; i++) {
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

		glm::mat4 model3;
		model3 = glm::translate(model3, glm::vec3(hedgelocX + 0.3 * i, 0.3, hedgelocZ));
		model3 = glm::scale(model3, glm::vec3(0.3, 0.4, 0.3));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
		
		glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

		glm::mat4 model4;
		model4 = glm::translate(model4, glm::vec3(hedgelocX + 0.3 * i, 0.05, hedgelocZ));
		model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

static void hedge2(GLint objectColorLoc, GLint modelLoc, float hedgelocX, float hedgelocZ, int hedgecount) {

	for (int i = 0; i < hedgecount; i++) {
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

		glm::mat4 model3;
		model3 = glm::translate(model3, glm::vec3(hedgelocX, 0.3, hedgelocZ + 0.3 * i));
		model3 = glm::scale(model3, glm::vec3(0.3, 0.4, 0.3));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

		glm::mat4 model4;
		model4 = glm::translate(model4, glm::vec3(hedgelocX, 0.05, hedgelocZ + 0.3 * i));
		model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

static void car(GLint objectColorLoc, GLint modelLoc, float carX, float carZ, int R, int G, int B) {
	//mobil
	glUniform3f(objectColorLoc, R / 255.0f, G / 255.0f, B / 255.0f);
	float carloc[2] = { carX ,carZ };
	//body
	glm::mat4 model5;
	model5 = glm::translate(model5, glm::vec3(carloc[0], 0.45, carloc[1]));
	model5 = glm::scale(model5, glm::vec3(1.1, 0.6, 3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//uppercar
	glm::mat4 model6;
	model6 = glm::translate(model6, glm::vec3(carloc[0], 0.95, carloc[1] + 0.25));
	model6 = glm::scale(model6, glm::vec3(1.1, 0.4, 1.8));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//windowcar
	glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

	glm::mat4 model13;
	model13 = glm::translate(model13, glm::vec3(carloc[0], 0.93, carloc[1] + 0.25));
	model13 = glm::scale(model13, glm::vec3(1, 0.3, 1.81));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model14;
	model14 = glm::translate(model14, glm::vec3(carloc[0], 0.93, carloc[1] + 0.25));
	model14 = glm::scale(model14, glm::vec3(1.11, 0.3, 1.65));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//plat
	glUniform3f(objectColorLoc, 171 / 255.0f, 171 / 255.0f, 3 / 255.0f);
	glm::mat4 model15;
	model15 = glm::translate(model15, glm::vec3(carloc[0], 0.4, carloc[1] + 1.505));
	model15 = glm::scale(model15, glm::vec3(0.28, 0.15, 0.01));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model16;
	model16 = glm::translate(model16, glm::vec3(carloc[0], 0.3, carloc[1] - 1.505));
	model16 = glm::scale(model16, glm::vec3(0.28, 0.15, 0.01));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	//wheel
	float wheloc[2] = { carloc[1] - 1, carloc[1] + 0.73 };

	for (int j = 0; j < 2; j++) {
		float weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + 0.06 + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.42, 0.18));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.3, 0.06));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.18, 0.06));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + 0.06 + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.42, 0.18));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.3, 0.06));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.18, 0.06));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	//carlight
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 252 / 255.0f, 252 / 255.0f, 0 / 255.0f);
		glm::mat4 headlight;
		headlight = glm::translate(headlight, glm::vec3(carloc[0] + 0.401 * pow(-1, i), 0.649, carloc[1] - 1.441));//carloc[0], 0.45, carloc[1]
		headlight = glm::scale(headlight, glm::vec3(0.3, 0.12, 0.12)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(headlight));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//brakes
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 180 / 255.0f, 0 / 255.0f, 0 / 255.0f);
		glm::mat4 brakes;
		brakes = glm::translate(brakes, glm::vec3(carloc[0] + 0.32 * pow(-1, i), 0.649, carloc[1] + 1.441));//carloc[0], 0.45, carloc[1]
		brakes = glm::scale(brakes, glm::vec3(0.15, 0.12, 0.12)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brakes));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//headlight
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 255 / 255.0f, 129 / 255.0f, 10 / 255.0f);
		glm::mat4 brakes;
		brakes = glm::translate(brakes, glm::vec3(carloc[0] + 0.47 * pow(-1, i), 0.649, carloc[1] + 1.441));//carloc[0], 0.45, carloc[1]
		brakes = glm::scale(brakes, glm::vec3(0.15, 0.12, 0.12)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brakes));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//exhaust
	glUniform3f(objectColorLoc, 105 / 255.0f, 105 / 255.0f, 105 / 255.0f);
	glm::mat4 exhaust;
	exhaust = glm::translate(exhaust, glm::vec3(carloc[0] + 0.4, 0.15, carloc[1] + 1.34));//carloc[0], 0.45, carloc[1]
	exhaust = glm::scale(exhaust, glm::vec3(0.08, 0.08, 0.4)); //1.1, 0.6, 3

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(exhaust));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

static void car2(GLint objectColorLoc, GLint modelLoc, float carX, float carZ, int R, int G, int B) {
	//mobil
	glUniform3f(objectColorLoc, R / 255.0f, G / 255.0f, B / 255.0f);
	float carloc[2] = { carX ,carZ };
	//body
	glm::mat4 model5;
	model5 = glm::translate(model5, glm::vec3(carloc[0], 0.45, carloc[1]));
	model5 = glm::scale(model5, glm::vec3(3, 0.6, 1.1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//uppercar
	glm::mat4 model6;
	model6 = glm::translate(model6, glm::vec3(carloc[0] + 0.25, 0.95, carloc[1]));
	model6 = glm::scale(model6, glm::vec3(1.8, 0.4, 1.1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//windowcar
	glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

	glm::mat4 model13;
	model13 = glm::translate(model13, glm::vec3(carloc[0] + 0.25, 0.93, carloc[1]));
	model13 = glm::scale(model13, glm::vec3(1.81, 0.3, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model14;
	model14 = glm::translate(model14, glm::vec3(carloc[0] + 0.25, 0.93, carloc[1]));
	model14 = glm::scale(model14, glm::vec3(1.65, 0.3, 1.11));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//plat
	glUniform3f(objectColorLoc, 171 / 255.0f, 171 / 255.0f, 3 / 255.0f);
	glm::mat4 model15;
	model15 = glm::translate(model15, glm::vec3(carloc[0] + 1.505, 0.4, carloc[1]));
	model15 = glm::scale(model15, glm::vec3(0.01, 0.15, 0.28));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 model16;
	model16 = glm::translate(model16, glm::vec3(carloc[0] - 1.505, 0.3, carloc[1]));
	model16 = glm::scale(model16, glm::vec3(0.01, 0.15, 0.28));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	float wheloc[2] = { carloc[0] - 1, carloc[0] + 0.73 };

	for (int j = 0; j < 2; j++) {
		float weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + 0.06 + weloc, 0.21, carloc[1] + 0.55));
				model7 = glm::scale(model7, glm::vec3(0.18, 0.42, 0.1));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + weloc, 0.21, carloc[1] + 0.55));
				model7 = glm::scale(model7, glm::vec3(0.06, 0.3, 0.1));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + weloc, 0.21, carloc[1] + 0.55));
				model7 = glm::scale(model7, glm::vec3(0.06, 0.18, 0.1));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + 0.06 + weloc, 0.21, carloc[1] - 0.55));
				model7 = glm::scale(model7, glm::vec3(0.18, 0.42, 0.1));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + weloc, 0.21, carloc[1] - 0.55));
				model7 = glm::scale(model7, glm::vec3(0.06, 0.3, 0.1));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(wheloc[j] + weloc, 0.21, carloc[1] - 0.55));
				model7 = glm::scale(model7, glm::vec3(0.06, 0.18, 0.1));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	//carlight
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 252 / 255.0f, 252 / 255.0f, 0 / 255.0f);
		glm::mat4 headlight;
		headlight = glm::translate(headlight, glm::vec3(carloc[0] - 1.441, 0.649, carloc[1] + 0.401 * pow(-1, i)));//carloc[0], 0.45, carloc[1]
		headlight = glm::scale(headlight, glm::vec3(0.12, 0.12, 0.3)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(headlight));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//brakes
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 180 / 255.0f, 0 / 255.0f, 0 / 255.0f);
		glm::mat4 brakes;
		brakes = glm::translate(brakes, glm::vec3(carloc[0] + 1.441, 0.649, carloc[1] + 0.32 * pow(-1, i)));//carloc[0], 0.45, carloc[1]
		brakes = glm::scale(brakes, glm::vec3(0.12, 0.12, 0.15)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brakes));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//headlight
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 255 / 255.0f, 129 / 255.0f, 10 / 255.0f);
		glm::mat4 brakes;
		brakes = glm::translate(brakes, glm::vec3(carloc[0] + 1.441, 0.649, carloc[1] + 0.47 * pow(-1, i)));//carloc[0], 0.45, carloc[1]
		brakes = glm::scale(brakes, glm::vec3(0.12, 0.12,0.15)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brakes));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//exhaust
	glUniform3f(objectColorLoc, 105 / 255.0f, 105 / 255.0f, 105 / 255.0f);
	glm::mat4 exhaust;
	exhaust = glm::translate(exhaust, glm::vec3(carloc[0] + 1.34, 0.15, carloc[1] + 0.4));//carloc[0], 0.45, carloc[1]
	exhaust = glm::scale(exhaust, glm::vec3( 0.4, 0.08, 0.08)); //1.1, 0.6, 3

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(exhaust));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

static void wall(GLint objectColorLoc, GLint modelLoc, float fenceX, float fenceZ, float fencelength, int rot) {

	//wall
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	if (rot == 0) {
		for (int i = 0; i < fencelength; i++) {
			glm::mat4 model10;
			model10 = glm::translate(model10, glm::vec3(fenceX + 1 * i, 0.35, fenceZ));
			model10 = glm::scale(model10, glm::vec3(1, 0.7, 0.3));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		for (int i = 0; i < fencelength; i++) {
			glm::mat4 model10;
			model10 = glm::translate(model10, glm::vec3(fenceX, 0.35, fenceZ + 1 * i));
			model10 = glm::scale(model10, glm::vec3(0.3, 0.7, 1));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}
}

static void deco(GLint objectColorLoc, GLint modelLoc, float fenceX, float fenceZ) {

	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

	glm::mat4 model9;
	model9 = glm::translate(model9, glm::vec3(fenceX, 0.45, fenceZ));
	model9 = glm::scale(model9, glm::vec3(0.4, 0.9, 0.4));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//deco
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	for (int j = 0; j < 3; j++) {

		glm::mat4 model8;
		model8 = glm::translate(model8, glm::vec3(fenceX, 0.15 - 0.05 * j, fenceZ));
		model8 = glm::scale(model8, glm::vec3(0.5 + j * 0.05, 0.05, 0.5 + j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model9;
		model9 = glm::translate(model9, glm::vec3(fenceX, 0.9, fenceZ));

		model9 = glm::scale(model9, glm::vec3(0.5 - j * 0.05, 0.15 + j * 0.05, 0.5 - j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

/*
static void fence(GLint objectColorLoc, GLint modelLoc, float fenceX, float fenceZ, float fencelength) {
	float fenceloc[2] = { fenceX, fenceZ };
	float wallmid = fencelength / 2;

	glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);

	glm::mat4 model9;
	model9 = glm::translate(model9, glm::vec3(fenceloc[0], 0.45, fenceloc[1]));
	model9 = glm::scale(model9, glm::vec3(0.4, 0.9, 0.4));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//deco
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	for (int j = 0; j < 3; j++) {

		glm::mat4 model8;
		model8 = glm::translate(model8, glm::vec3(fenceloc[0], 0.15 - 0.05 * j, fenceloc[1]));
		model8 = glm::scale(model8, glm::vec3(0.5 + j * 0.05, 0.05, 0.5 + j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model9;
		model9 = glm::translate(model9, glm::vec3(fenceloc[0], 0.9, fenceloc[1]));

		model9 = glm::scale(model9, glm::vec3(0.5 - j * 0.05, 0.15 + j * 0.05, 0.5 - j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//wall
	glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);

	glm::mat4 model10;
	model10 = glm::translate(model10, glm::vec3(fenceloc[0] + wallmid, 0.35, fenceloc[1]));
	model10 = glm::scale(model10, glm::vec3(fencelength, 0.7, 0.3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


}
*/

static void Road(GLint objectColorLoc, GLint modelLoc, float roadX, float roadZ, float roadsegmen)
{
	int counter = 0;
	//road
	for (int i = 0; i < roadsegmen; i++) {
		
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);

		glm::mat4 model20;
		model20 = glm::translate(model20, glm::vec3(roadX + 1 * i, 0.025, roadZ));
		model20 = glm::scale(model20, glm::vec3(1, 0.05, 3.4));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model20));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//sideline

		glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);

		glm::mat4 model23;
		model23 = glm::translate(model23, glm::vec3(roadX + 1 * i, 0.06, roadZ + 1.55));
		model23 = glm::scale(model23, glm::vec3(1, 0.01, 0.1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model23));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model24;
		model24 = glm::translate(model24, glm::vec3(roadX + 1 * i, 0.06, roadZ - 1.55));
		model24 = glm::scale(model24, glm::vec3(1, 0.01, 0.1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model24));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//stripes
		if (i % 2 == 1) {

			glm::mat4 model21;

			model21 = glm::translate(model21, glm::vec3(roadX + i * 1, 0.06, roadZ));
			model21 = glm::scale(model21, glm::vec3(0.9, 0.01, 0.1));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		//side
		for (int j = 0; j < 2; j++) {
			if (counter % 2 == 0) {
				glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);
			}
			else {
				glUniform3f(objectColorLoc, 15 / 255.0f, 15 / 255.0f, 15 / 255.0f);
			}
			counter++;
			glm::mat4 model22;

			model22 = glm::translate(model22, glm::vec3(roadX - 0.25 + 0.5 * j + i * 1, 0.05, roadZ + 1.85));
			model22 = glm::scale(model22, glm::vec3(0.5, 0.1, 0.3));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			glm::mat4 model25;

			model25 = glm::translate(model25, glm::vec3(roadX - 0.25 + 0.5 * j + i * 1, 0.05, roadZ - 1.85));
			model25 = glm::scale(model25, glm::vec3(0.5, 0.1, 0.3));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model25));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}
}

static void Road2(GLint objectColorLoc, GLint modelLoc, float roadX, float roadZ, float roadsegmen)
{
	int counter = 0;
	//road
	for (int i = 0; i < roadsegmen; i++) {

		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);

		glm::mat4 model20;
		model20 = glm::translate(model20, glm::vec3(roadX, 0.025, roadZ + 1 * i));
		model20 = glm::scale(model20, glm::vec3(3.4, 0.05,1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model20));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//sideline

		glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);

		glm::mat4 model23;
		model23 = glm::translate(model23, glm::vec3(roadX + 1.55, 0.06, roadZ + 1 * i));
		model23 = glm::scale(model23, glm::vec3(0.1, 0.01, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model23));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model24;
		model24 = glm::translate(model24, glm::vec3(roadX - 1.55, 0.06, roadZ + 1 * i));
		model24 = glm::scale(model24, glm::vec3(0.1, 0.01, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model24));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//stripes
		if (i % 2 == 1) {

			glm::mat4 model21;

			model21 = glm::translate(model21, glm::vec3(roadX, 0.06, roadZ + i * 1));
			model21 = glm::scale(model21, glm::vec3(0.1, 0.01, 0.9));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		//side
		for (int j = 0; j < 2; j++) {
			if (counter % 2 == 0) {
				glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);
			}
			else {
				glUniform3f(objectColorLoc, 15 / 255.0f, 15 / 255.0f, 15 / 255.0f);
			}
			counter++;
			glm::mat4 model22;

			model22 = glm::translate(model22, glm::vec3(roadX + 1.85, 0.05, roadZ - 0.25 + 0.5 * j + i * 1));
			model22 = glm::scale(model22, glm::vec3(0.3, 0.1, 0.5));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			glm::mat4 model25;

			model25 = glm::translate(model25, glm::vec3(roadX - 1.85, 0.05, roadZ - 0.25 + 0.5 * j + i * 1));
			model25 = glm::scale(model25, glm::vec3(0.3, 0.1, 0.5));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model25));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}
}

static void intersection(GLint objectColorLoc, GLint modelLoc, float interX, float interY) {

	glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);

	glm::mat4 model20;
	model20 = glm::translate(model20, glm::vec3(interX, 0.025, interY));
	model20 = glm::scale(model20, glm::vec3(4, 0.05, 4));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model20));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//stripes
	glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);
	int counter = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			if (i == 0 || i == 3) {
				glm::mat4 stripes;
				stripes = glm::translate(stripes, glm::vec3(interX - 1.5 - 0.1 * pow(-1, counter) + 1 * i, 0.06, interY + 1.55 * pow(-1, j)));
				stripes = glm::scale(stripes, glm::vec3(0.4, 0.01, 0.1));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stripes));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 stripes2;
				stripes2 = glm::translate(stripes2, glm::vec3(interX + 1.55 * pow(-1, j), 0.06, interY - 1.5 - 0.1 * pow(-1, counter) + 1 * i));
				stripes2 = glm::scale(stripes2, glm::vec3(0.1, 0.01, 0.4));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stripes2));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

			} else {
				glm::mat4 stripes;
				stripes = glm::translate(stripes, glm::vec3(interX - 1.5 + 1 * i, 0.06, interY + 1.55 * pow(-1, j)));
				stripes = glm::scale(stripes, glm::vec3(0.8, 0.01, 0.1));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stripes));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				
				glm::mat4 stripes2;
				stripes2 = glm::translate(stripes2, glm::vec3(interX + 1.55 * pow(-1, j), 0.06, interY - 1.5 + 1 * i));
				stripes2 = glm::scale(stripes2, glm::vec3(0.1, 0.01, 0.9));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stripes2));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				counter = 2;
			}
			/*
			glm::mat4 stripes2;
			stripes2 = glm::translate(stripes2, glm::vec3(interX + 1.55 * pow(-1, j), 0.06, interY + 1 * i));
			stripes2 = glm::scale(stripes2, glm::vec3(0.1, 0.01, 0.9));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(stripes2));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			*/
		}
	}
}

static void intersectionside(GLint objectColorLoc, GLint modelLoc, float interX, float interY, int start) {
	
	int counter = start;
	for (int i = 0; i < 8; i++) {
		if (counter % 2 == 0) {
			glUniform3f(objectColorLoc, 15 / 255.0f, 15 / 255.0f, 15 / 255.0f);
		}
		else {
			glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);
		}
		counter++;
		glm::mat4 model22;

		model22 = glm::translate(model22, glm::vec3(interX + 0.5 * i, 0.05, interY));
		model22 = glm::scale(model22, glm::vec3(0.5, 0.1, 0.3));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

static void intersectionside2(GLint objectColorLoc, GLint modelLoc, float interX, float interY, int start) {

	int counter = start;
	for (int i = 0; i < 8; i++) {
		if (counter % 2 == 0) {
			glUniform3f(objectColorLoc, 15 / 255.0f, 15 / 255.0f, 15 / 255.0f);
		}
		else {
			glUniform3f(objectColorLoc, 204 / 255.0f, 204 / 255.0f, 204 / 255.0f);
		}
		counter++;
		glm::mat4 model22;

		model22 = glm::translate(model22, glm::vec3(interX, 0.05, interY + 0.5 * i));
		model22 = glm::scale(model22, glm::vec3( 0.3, 0.1, 0.5));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

static void Roadwalk(GLint objectColorLoc, GLint modelLoc, float roadX, float roadZ, float roadsegmen) {

	for (int i = 0; i < roadsegmen; i++) {
		glUniform3f(objectColorLoc, 105 / 255.0f, 105 / 255.0f, 105 / 255.0f);
		glm::mat4 roadwalk;
		roadwalk = glm::translate(roadwalk, glm::vec3(roadX + 1 * i, 0.075, roadZ));
		roadwalk = glm::scale(roadwalk, glm::vec3(1, 0.15, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadwalk));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform3f(objectColorLoc, 134 / 255.0f, 134 / 255.0f, 134 / 255.0f);
		glm::mat4 roadfill;
		roadfill = glm::translate(roadfill, glm::vec3(roadX + 1 * i, 0.075, roadZ));
		roadfill = glm::scale(roadfill, glm::vec3(0.95, 0.1501, 0.95));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 roadfill2;
		roadfill2 = glm::translate(roadfill2, glm::vec3(roadX + 1 * i, 0.075, roadZ));
		roadfill2 = glm::scale(roadfill2, glm::vec3(1.001, 0.12, 0.95));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill2));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 roadfill3;
		roadfill3 = glm::translate(roadfill3, glm::vec3(roadX + 1 * i, 0.075, roadZ));
		roadfill3 = glm::scale(roadfill3, glm::vec3(0.95, 0.12, 1.001));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill3));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
}

static void Roadwalk2(GLint objectColorLoc, GLint modelLoc, float roadX, float roadZ, float roadsegmen) {

	for (int i = 0; i < roadsegmen; i++) {
		glUniform3f(objectColorLoc, 105 / 255.0f, 105 / 255.0f, 105 / 255.0f);
		glm::mat4 roadwalk;
		roadwalk = glm::translate(roadwalk, glm::vec3(roadX, 0.075, roadZ + 1 * i));
		roadwalk = glm::scale(roadwalk, glm::vec3(1, 0.15, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadwalk));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform3f(objectColorLoc, 134 / 255.0f, 134 / 255.0f, 134 / 255.0f);
		glm::mat4 roadfill;
		roadfill = glm::translate(roadfill, glm::vec3(roadX , 0.075, roadZ + 1 * i));
		roadfill = glm::scale(roadfill, glm::vec3(0.95, 0.1501, 0.95));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 roadfill2;
		roadfill2 = glm::translate(roadfill2, glm::vec3(roadX, 0.075, roadZ + 1 * i));
		roadfill2 = glm::scale(roadfill2, glm::vec3(1.001, 0.12, 0.95));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill2));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 roadfill3;
		roadfill3 = glm::translate(roadfill3, glm::vec3(roadX, 0.075, roadZ + 1 * i));
		roadfill3 = glm::scale(roadfill3, glm::vec3(0.95, 0.12, 1.001));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(roadfill3));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

static void Building(GLint objectColorLoc, GLint modelLoc, float buildingX, float buildingZ, int storiesHigh, int R, int G, int B) {

	//bangunan 1
	for (int i = 0; i < storiesHigh; i++) {
		glUniform3f(objectColorLoc, R / 255.0f, G / 255.0f, B / 255.0f);

		glm::mat4 model12;
		model12 = glm::translate(model12, glm::vec3(buildingX, 1 + i * 2, buildingZ));
		model12 = glm::scale(model12, glm::vec3(4.5, 2, 4.5));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model12));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);
	
	//pintu
	glm::mat4 model11;

	model11 = glm::translate(model11, glm::vec3(buildingX, 0.9, buildingZ));
	model11 = glm::scale(model11, glm::vec3(1.2, 1.8, 4.51));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glm::mat4 DOR;

	DOR = glm::translate(DOR, glm::vec3(buildingX, 0.9, buildingZ));
	DOR = glm::scale(DOR, glm::vec3(4.51, 1.8, 1.2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(DOR));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//jendela depan belakang
	for (int i = 0; i < storiesHigh-1; i++) {
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model13;
		model13 = glm::translate(model13, glm::vec3(buildingX - 1.4, 3 + i * 2, buildingZ));
		model13 = glm::scale(model13, glm::vec3(1, 1, 4.51));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model14;
		model14 = glm::translate(model14, glm::vec3(buildingX + 1.4, 3 + i * 2, buildingZ));
		model14 = glm::scale(model14, glm::vec3(1, 1, 4.51));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//jendela kiri kanan
	for (int i = 0; i < storiesHigh-1; i++) {
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model15;
		model15 = glm::translate(model15, glm::vec3(buildingX, 3 + i * 2, buildingZ + 1.6));

		model15 = glm::scale(model15, glm::vec3(4.51, 1, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model16;
		model16 = glm::translate(model16, glm::vec3(buildingX, 3 + i * 2, buildingZ - 1.6));
		model16 = glm::scale(model16, glm::vec3(4.51, 1, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	}
}

static void Building2(GLint objectColorLoc, GLint modelLoc, float buildingX, float buildingZ, int storiesHigh, int R, int G, int B) {

	for (int i = 0; i < storiesHigh; i++) {
		glUniform3f(objectColorLoc, R / 255.0f, G / 255.0f, B / 255.0f);

		glm::mat4 model12;
		model12 = glm::translate(model12, glm::vec3(buildingX, 1 + i * 2, buildingZ));
		model12 = glm::scale(model12, glm::vec3(4.5, 2, 4.5));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model12));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);
	
	//pintu
	glm::mat4 model11;

	model11 = glm::translate(model11, glm::vec3(buildingX , 0.9, buildingZ));
	model11 = glm::scale(model11, glm::vec3(1.2, 1.8, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glm::mat4 dor;

	dor = glm::translate(dor, glm::vec3(buildingX + 0.06, 0.9, buildingZ + 2.26));
	dor = glm::scale(dor, glm::vec3(1.2, 1.8, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(dor));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//jendela depan belakang
	for (int i = 0; i < storiesHigh - 1; i++) {
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model13;
		model13 = glm::translate(model13, glm::vec3(buildingX - 1.4, 3 + i * 1.9, buildingZ));
		model13 = glm::scale(model13, glm::vec3(1, 1, 4.51));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model14;
		model14 = glm::translate(model14, glm::vec3(buildingX + 1.4, 3 + i * 1.9, buildingZ));
		model14 = glm::scale(model14, glm::vec3(1, 1, 4.51));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//jendela kiri kanan
	for (int i = 0; i < storiesHigh; i++) {
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model15;
		model15 = glm::translate(model15, glm::vec3(buildingX, 1.1 + i * 1.9, buildingZ + 1.6));

		model15 = glm::scale(model15, glm::vec3(4.51, 1, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model16;
		model16 = glm::translate(model16, glm::vec3(buildingX, 1.1 + i * 1.9, buildingZ - 1.6));
		model16 = glm::scale(model16, glm::vec3(4.51, 1, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	}
}

static void tugu(GLint objectColorLoc, GLint modelLoc, float tuguX, float tuguZ) {
	//badan tugu
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

	glm::mat4 model21;

	model21 = glm::translate(model21, glm::vec3(tuguX, 1.25, tuguZ));
	/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
	model21 = glm::scale(model21, glm::vec3(0.7, 2.5, 0.7));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//alas tugu
	for (int i = 0; i < 2; i++) {
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(tuguX, 0.15 + i * 0.3, tuguZ));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(1.5 - i * 0.2, 0.3, 1.5 - i * 0.2));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//atap tugu
	for (int i = 0; i < 5; i++) {
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(tuguX, 2.55 + i * 0.1, tuguZ));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.63 - i * 0.07, 0.1, 0.63 - i * 0.07));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
}

/*
void Demo::DrawYard()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 0.0 / 255.0f, 185.0 / 255.0f, 0.0 / 255.0f);

	glm::mat4 model22;
	model22 = glm::translate(model22, glm::vec3(5, 0, 5));
	model22 = glm::scale(model22, glm::vec3(20, 0, 15));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//garis
	for (int i = 0; i < 11; i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(14 - i * 1.8, 0.1, -4));
		//model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));
		model21 = glm::scale(model21, glm::vec3(0.9, 0, 0.1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}



	//tugu
	for (int i = 0; i < 1; i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 1.5 + i * 2, 1));
		//model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));
		model21 = glm::scale(model21, glm::vec3(0.7, 2.5, 0.7));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//alas tugu
	for (int i = 0; i < 1; i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 0.15 + i * 2, 1));
		//model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));
		model21 = glm::scale(model21, glm::vec3(1.5, 0.3, 1.5));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//atap tugu1
	for (int i = 0; i < 1; i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 3.1 + i * 2, 1));
		//model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));
		model21 = glm::scale(model21, glm::vec3(0.5, 0.2, 0.5));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//atap tugu2
	for (int i = 0; i < 1; i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 3 + i * 2, 1));
		//model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));
		model21 = glm::scale(model21, glm::vec3(0.1, 1, 0.1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}
*/

//static void gedung(GLint objectColorLoc, GLint modelLoc, float gedungX, float gedungZ) {
//	//mobil
//	glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
//	float gedungloc[2] = { gedungX ,gedungZ };
//	//body
//	glm::mat4 model11;
//	model11 = glm::translate(model11, glm::vec3(gedungloc[0], 0.75, gedungloc[1]));
//	model11 = glm::scale(model11, glm::vec3(1.1, 0.6, 3));
//
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));
//
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
//
//
//	}

void Demo::DrawTexturedCube() {
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");

	//hedge(objectColorLoc, modelLoc, 1.25, 1, 10); //posX, posY, hedgecount

	car(objectColorLoc, modelLoc, -7.5, 10.5, 255,0,0);
	car(objectColorLoc, modelLoc, -7.5, -0.5, 160, 82, 45);
	car2(objectColorLoc, modelLoc, 20, 21, 250, 236, 35);
	car2(objectColorLoc, modelLoc, 0.5, -3.2, 0, 0, 255);

	//fence(objectColorLoc, modelLoc, 0, -2, 15);
	//fence(objectColorLoc, modelLoc, -5, -6, 20);

	//south
	Road(objectColorLoc, modelLoc, -5, -4, 21);//4.0 2.0
	Roadwalk(objectColorLoc, modelLoc, -5, -1.5, 20);
	Roadwalk(objectColorLoc, modelLoc, -10, -6.5, 30);//7
	intersection(objectColorLoc, modelLoc, 17.5, -4);
	intersectionside(objectColorLoc, modelLoc, 15.75, -5.85, 1); //1.75 , 1.85
	intersectionside2(objectColorLoc, modelLoc, 19.35, -5.75, 0); //1.85, 1.75
	deco(objectColorLoc, modelLoc, -4.35, -0.85);
	wall(objectColorLoc, modelLoc, -4, -0.85, 8, 0); //0.3
	deco(objectColorLoc, modelLoc, 3.5, -0.85);
	deco(objectColorLoc, modelLoc, 6.5, -0.85);
	wall(objectColorLoc, modelLoc, 7, -0.85, 8, 0);
	Building(objectColorLoc, modelLoc, -8.25, -9.25, 16, 242, 36, 22);//4.5 2.25
	Building(objectColorLoc, modelLoc, -3.75, -9.25, 5, 2, 229, 237);
	Building(objectColorLoc, modelLoc, 0.75, -9.25, 8, 114, 247, 140);
	Building(objectColorLoc, modelLoc, 5.25, -9.25, 6, 184, 172, 6);//4.5 2.25
	Building(objectColorLoc, modelLoc, 9.75, -9.25, 9, 255, 255, 255);
	Building(objectColorLoc, modelLoc, 14.25, -9.25, 9, 252, 5, 121);
	Building(objectColorLoc, modelLoc, 18.75, -9.25, 9, 62, 122, 5);//4.5 2.25

	//west
	Road2(objectColorLoc, modelLoc, 17.5, -1.5, 21);//1.84
	Roadwalk2(objectColorLoc, modelLoc, 15, -1.5, 20);
	Roadwalk2(objectColorLoc, modelLoc, 20, -6.5, 30);
	intersection(objectColorLoc, modelLoc, 17.5, 21);
	intersectionside(objectColorLoc, modelLoc, 15.75, 22.85, 1); //1.75 , 1.85
	intersectionside2(objectColorLoc, modelLoc, 19.35, 19.25, 1); //1.85, 1.75
	deco(objectColorLoc, modelLoc, 14.35, -0.85);
	wall(objectColorLoc, modelLoc, 14.35, -0.5, 8, 1); //0.3
	deco(objectColorLoc, modelLoc, 14.35, 7);
	deco(objectColorLoc, modelLoc, 14.35, 10);
	wall(objectColorLoc, modelLoc, 14.35, 10.5, 8, 1);
	Building(objectColorLoc, modelLoc, 22.75, -4.75, 8, 255, 169, 56);//4.5 2.25
	Building(objectColorLoc, modelLoc, 22.75, -0.25, 16, 97, 97, 97);
	Building(objectColorLoc, modelLoc, 22.75, 4.25, 3, 73, 115, 252);
	Building(objectColorLoc, modelLoc, 22.75, 8.75, 4, 3, 84, 75);//4.5 2.25
	Building(objectColorLoc, modelLoc, 22.75, 13.25, 20, 84, 3, 32);
	Building(objectColorLoc, modelLoc, 22.75, 17.75, 7, 102, 138, 123);
	Building(objectColorLoc, modelLoc, 22.75, 22.25, 10, 160, 82, 45);//4.5 2.25

	//north
	Road(objectColorLoc, modelLoc, -5, 21, 21);//1.84
	Roadwalk(objectColorLoc, modelLoc, -4, 18.5, 20);
	Roadwalk(objectColorLoc, modelLoc, -9, 23.5, 30);
	intersection(objectColorLoc, modelLoc, -7.5, 21);
	intersectionside(objectColorLoc, modelLoc, -9.25, 22.85, 0); //1.75 , 1.85
	intersectionside2(objectColorLoc, modelLoc, -9.35, 19.25, 1); //1.85, 1.75
	deco(objectColorLoc, modelLoc, 14.35, 17.85);
	wall(objectColorLoc, modelLoc, 7, 17.85, 8, 0);
	deco(objectColorLoc, modelLoc, 6.5, 17.85);
	deco(objectColorLoc, modelLoc, 3.5, 17.85);
	wall(objectColorLoc, modelLoc, -4, 17.85, 8, 0); //0.3
	Building(objectColorLoc, modelLoc, -8.25, 26.25, 10, 160, 82, 45);//4.5 2.25
	Building(objectColorLoc, modelLoc, -3.75, 26.25, 4, 5, 12, 200);
	Building(objectColorLoc, modelLoc, 0.75, 26.25, 8, 236, 194, 146);
	Building(objectColorLoc, modelLoc, 5.25, 26.25, 5, 173, 67, 5);//4.5 2.25
	Building(objectColorLoc, modelLoc, 9.75, 26.25, 13, 178, 5, 252);
	Building(objectColorLoc, modelLoc, 14.25, 26.25, 23, 35, 250, 56);
	Building(objectColorLoc, modelLoc, 18.75, 26.25, 23, 250, 236, 35);//4.5 2.25

	//east
	Road2(objectColorLoc, modelLoc, -7.5, -1.5, 21);//1.84
	Roadwalk2(objectColorLoc, modelLoc, -5, -0.5, 20);
	Roadwalk2(objectColorLoc, modelLoc, -10, -5.5, 30);
	intersection(objectColorLoc, modelLoc, -7.5, -4);
	intersectionside(objectColorLoc, modelLoc, -9.25, -5.85, 1); //1.75 , 1.85
	intersectionside2(objectColorLoc, modelLoc, -9.35, -5.75, 1); //1.85, 1.75
	deco(objectColorLoc, modelLoc, -4.35, 17.85);
	wall(objectColorLoc, modelLoc, -4.35, -0.5, 8, 1); //0.3
	deco(objectColorLoc, modelLoc, -4.35, 7);
	deco(objectColorLoc, modelLoc, -4.35, 10);
	wall(objectColorLoc, modelLoc, -4.35, 10.5, 8, 1);
	Building(objectColorLoc, modelLoc, -12.75, -4.75, 8, 255, 171, 216);//4.5 2.25
	Building(objectColorLoc, modelLoc, -12.75, -0.25, 6, 14, 9, 102);
	Building(objectColorLoc, modelLoc, -12.75, 4.25, 5, 158, 196, 92);
	Building(objectColorLoc, modelLoc, -12.75, 8.75, 15, 81, 93, 115);//4.5 2.25
	Building(objectColorLoc, modelLoc, -12.75, 13.25, 16, 250, 97, 110);
	Building(objectColorLoc, modelLoc, -12.75, 17.75, 5, 245, 33, 5);
	Building(objectColorLoc, modelLoc, -12.75, 22.25, 3, 255, 66, 195);//4.5 2.25

	//Building(objectColorLoc, modelLoc, 5.5, 4, 3, 160, 82, 45);
	//Building(objectColorLoc, modelLoc, 11, 4, 5, 5, 12, 200);


	//citysquare
	tugu(objectColorLoc, modelLoc, 5, 8.5);
	Roadwalk2(objectColorLoc, modelLoc, 5, -0.5, 19);
	Roadwalk2(objectColorLoc, modelLoc, 4, -0.5, 19);
	Roadwalk2(objectColorLoc, modelLoc, 6, -0.5, 19);

	for (int i = 0; i < 4; i++) {
		tree(objectColorLoc, modelLoc, 6.9, 0.7 + 1.4 * i);
		tree(objectColorLoc, modelLoc, 3.1, 0.7 + 1.4 * i);
	}

	//tree(objectColorLoc, modelLoc, -2, 3); 
	//hedge(objectColorLoc, modelLoc, 1.25, 1, 5);
	//hedge(objectColorLoc, modelLoc, -2.5, 4, 7);
	//hedge(objectColorLoc, modelLoc, 5, 2, 10);

	Roadwalk(objectColorLoc, modelLoc, -4, 7.5, 19);
	Roadwalk(objectColorLoc, modelLoc, -4, 8.5, 19);
	Roadwalk(objectColorLoc, modelLoc, -4, 9.5, 19);

	for (int i = 0; i < 3; i++) {
		tree(objectColorLoc, modelLoc, -3.5 + 1.8 * i, 5);
		tree(objectColorLoc, modelLoc, -2.5 + 1.6 * i, 3);
	}

	hedge(objectColorLoc, modelLoc, -3.5, 6.7, 18);
	hedge(objectColorLoc, modelLoc, -3.5, 10.3, 6);
	hedge(objectColorLoc, modelLoc, 0, 10.3, 10);

	tree(objectColorLoc, modelLoc, -3, 15);
	hedge2(objectColorLoc, modelLoc, 3, 12, 18);

	hedge2(objectColorLoc, modelLoc, 7, 11, 14);
	hedge2(objectColorLoc, modelLoc, 7, 17, 6);
	hedge(objectColorLoc, modelLoc, 7, 10.3, 18);

	for (int i = 0; i < 3; i++) {
		tree(objectColorLoc, modelLoc, 8 + 2 * i, 11);
		tree(objectColorLoc, modelLoc, 9.7 + 1.6 * i, 13);
		tree(objectColorLoc, modelLoc, 8.6 + 1.8 * i, 16);
	}


	for (int i = 0; i < 4; i++) {
		tree(objectColorLoc, modelLoc, 7 + 2 * i, 6);
	}

	glBindVertexArray(0);

}

void Demo::BuildTexturedPlane()
{
	// Build geometry
	GLfloat vertices[] = {
		// format position, normals
		// bottom
		-50.0, -0.5, -50.0,  0.0f,  1.0f,  0.0f,
		 50.0, -0.5, -50.0,  0.0f,  1.0f,  0.0f,
		 50.0, -0.5,  50.0,  0.0f,  1.0f,  0.0f,
		-50.0, -0.5,  50.0,  0.0f,  1.0f,  0.0f,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawTexturedPlane()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 0.0 / 255.0f, 185.0 / 255.0f, 0.0 / 255.0f);

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.002f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Project Team Geeks", 1920, 1080, true, true);
}