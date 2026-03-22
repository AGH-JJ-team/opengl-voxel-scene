#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.hpp"
#include"shaderClass.hpp"
#include"VAO.hpp"
#include"VBO.hpp"
#include"EBO.hpp"
#include"Camera.hpp"
#include"Model.hpp"
#include"Skybox.hpp"
#include"FrameBuffer.hpp"


// Rozdzielczo�� okna
const unsigned int width = 1920;
const unsigned int height = 1080;



// Wierzcho�ki dla sze�cianu �wiat�a
GLfloat lightVertices[] =
{ // COORDINATES //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};


// Indeksy tr�jk�t�w dla sze�cianu �wiat�a
GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


// Wierzcho�ki dla prostok�ta u�ywanego do post-processingu (framebuffera)
float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


int main()
{
	// Inicjalizacja GLFW i ustawienia kontekstu OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tworzenie okna
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Inicjalizacja GLAD i ustawienie widoku
	gladLoadGL();
	glViewport(0, 0, width, height);

	// W��czenie przezroczysto�ci
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Za�adowanie shader�w (program g��wny, skybox, framebuffer i �wiat�o)
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader framebufferProgram("framebuffer.vert", "framebuffer.frag");
	Shader lightShader("light.vert", "light.frag");
	
	// Za�adowanie tekstury i przypisanie do shadera
	Texture dirtTex("textures/combined_minecraft_texture.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	dirtTex.texUnit(shaderProgram, "texture1", 0);
	
	// Inicjalizacja kamery
	Camera camera(width, height, glm::vec3(15.0f, 0.0f, 30.0f));

	
	// Za�adowanie modeli wyspy, ska�y (do�u wyspy) i wiatrak�w
	Model island;
	if (!island.LoadFromFile("models/island/island.obj")) {
		std::cerr << "Failed to load model" << std::endl;
		return -1;
	}
	island.SetupMesh();

	Model stone;
	if (!stone.LoadFromFile("models/island/stone_island.obj")) {
		std::cerr << "Failed to load model" << std::endl;
		return -1;
	}
	stone.SetupMesh();

	Model wiatrak1;
	if (!wiatrak1.LoadFromFile("models/island/wiatrak_1.obj")) {
		std::cerr << "Failed to load model" << std::endl;
		return -1;
	}
	wiatrak1.SetupMesh();

	Model wiatrak2;
	if (!wiatrak2.LoadFromFile("models/island/wiatrak_2.obj")) {
		std::cerr << "Failed to load model" << std::endl;
		return -1;
	}
	wiatrak2.SetupMesh();

	// Pozycje modeli
	glm::vec3 islandPos(0.0f, 0.0f, 0.0f);
	glm::vec3 stonePos(-0.0f, 0.0f, 0.0f);
	glm::vec3 wiatrak1Pos(-1.0f, 14.0f, 0.0f);
	glm::vec3 wiatrak2Pos(39.9691f, 17.0f, 0.0f);
	
	// Tworzenie VAO/VBO/EBO dla obiektu �wiat�a
	VAO lightVAO;
	lightVAO.Bind();
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();


	// Pozycja i kolor �wiat�a
	glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	
	
	// Ustawienie shadera obiekt�w, �wiat�a, skyboxa i framebuffera
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	
	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.ID, "screenTexture"), 0);

	
	// Tworzenie VAO i VBO dla prostok�ta post-processingu
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	

	// �cie�ki do tekstur skyboxa
	std::vector<std::string> facesCubemap =
	{
		"Skybox/px.png",
		"Skybox/nx.png",
		"Skybox/py.png",
		"Skybox/ny.png",
		"Skybox/pz.png",
		"Skybox/nz.png"
	};
	
	// Inicjalizacja skyboxa i framebuffera
	Skybox skybox(facesCubemap);
	FrameBuffer framebuffer(width, height);

	

	// G��wna p�tla renderowania
	while (!glfwWindowShouldClose(window))
	{
		// Rysowanie do w�asnego framebuffera (efekty post-processingu)
		framebuffer.Bind();
		
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		// Obs�uga wej�cia kamery i efekt�w
		camera.Inputs(window);
		framebuffer.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Animacja �wiat�a (ruch po elipsie)
		static float lightAngle = 0.0f;
		lightAngle += 0.003f;
		glm::vec3 lightPos = glm::vec3(50.0f * cos(lightAngle) + 16.0f, 50.0f * sin(lightAngle), sin(lightAngle) * 50.0f);
		glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
		shaderProgram.Activate();

		// Ustawienie macierzy modelu
		glm::mat4 modelMatrix = glm::mat4(0.5f);
		glm::mat4 modelMatrix1 = glm::mat4(0.5f);
		
		// Ustawienie pozycji kamery i �wiat�a oraz kolor�w �wiat�a i jego �r�d�a
		camera.Matrix(shaderProgram, "camMatrix");
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "objectColor"), 1.0f, 0.5f, 0.31f);

		
		// Renderowanie wyspy i element�w
		dirtTex.Bind();
		float rotationSpeed = 1.0f;
		float rotationAngle = (float)glfwGetTime() * rotationSpeed;


		// Wyspa
		modelMatrix = glm::translate(modelMatrix, islandPos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		island.Render();

		// Kamie�
		modelMatrix = glm::translate(modelMatrix, stonePos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		stone.Render();

		// Wiatrak 1 z animacj�
		modelMatrix = glm::translate(modelMatrix, wiatrak1Pos);
		modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::translate(modelMatrix, -wiatrak1Pos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		wiatrak1.Render();

		// Wiatrak 2 z animacj�
		modelMatrix1 = glm::translate(modelMatrix1, wiatrak2Pos);
		modelMatrix1 = glm::rotate(modelMatrix1, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix1 = glm::translate(modelMatrix1, -wiatrak2Pos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix1));
		wiatrak2.Render();


		// Renderowanie �wiat�a
		lightShader.Activate();
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		camera.Matrix(lightShader, "camMatrix");
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		lightVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		
		// Renderowanie skyboxa
		skybox.Draw(camera, skyboxShader, width, height);


		// Przypisywanie domy�lnego framebuffera
		framebuffer.Unbind();
		// Rysowanie prostok�ta framebuffera
		framebufferProgram.Activate();
		glBindVertexArray(rectVAO);
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "invertColors"), framebuffer.invertColors); //Sprawdzenie przyci�ni�cia przycisku R (negatyw)
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "blackAndWhite"), framebuffer.blackAndWhite); //Sprawdzenie przyci�ni�cia przycisku T (czarno-bia�y)
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "redFilter"), framebuffer.redFilter); //Sprawdzenie przyci�ni�cia przycisku Y (nasycony czerwieni�)
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "greenFilter"), framebuffer.greenFilter); //Sprawdzenie przyci�ni�cia przycisku U (nasycony zieleni�)
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "blueFilter"), framebuffer.blueFilter); //Sprawdzenie przyci�ni�cia przycisku I (nasycony niebieskim)
		glUniform1i(glGetUniformLocation(framebufferProgram.ID, "border"), framebuffer.border); //Sprawdzenie przyci�ni�cia przycisku O (wykrywanie kraw�dzi)
		glDisable(GL_DEPTH_TEST);
		framebuffer.BindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Aktualizacja ekranu
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Zwolnienie zasob�w
	dirtTex.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}