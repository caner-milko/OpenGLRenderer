#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

#include <GLTypes/Shader.h>
#include <GLTypes/Texture.h>
#include <GLTypes/Renderer.h>
#include <GLTypes/Camera.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
double texOffsetx = 0.0f, texOffsety = 0.0f;

double deltaTime = 0.0f;	// Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame

FreeCamera *camera;

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);

float yaw = -90.0f, pitch = 0.0f;
double lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

int main()
{


	/*---------------------------------------------------*/
	//Init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*---------------------------------------------------*/

	std::vector<float> vertices = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	std::vector<float> vertices2 = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2,3,0
	};

	std::vector<uint32_t> attributeSizes = {3,2};

	Renderer *renderer = new Renderer(RendererData());

	camera = renderer->camera;

	camera->setCameraPos(glm::vec3(0.0f, 0.0f, 3.0f));

	/*---------------------------------------------------*/
	//textures

	Texture2D *textures[2] = {new Texture2D("./Assets/wall.jpg", false, TextureType::RGB, TextureType::RGB),new Texture2D("./Assets/awesomeface.png", true, TextureType::RGBA, TextureType::RGBA)};
	/*---------------------------------------------------*/
	//shaders
	// 
	//Shader *shader = new Shader("./Assets/Shaders/def_vert.glsl", "./Assets/Shaders/def_frag.glsl");
	Shader *shader = new Shader("./Assets/Shaders/def_vert.glsl", "./Assets/Shaders/def_frag.glsl");
	shader->addTexture("texture1", textures[0]);
	shader->addTexture("texture2", textures[1]);
	/*---------------------------------------------------*/

	RenderObject *quad = renderer->addMesh(RenderObjectData(), shader, vertices2, attributeSizes, indices);



	RenderObject *odd = renderer->addOddObject(RenderObjectData(), shader, vertices, attributeSizes);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::vector<glm::vec3> cubePositions = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	quad->setPosition(cubePositions[1]);

	odd->setPosition(cubePositions[0]);

	std::vector<RenderObject *> objects;
	objects.push_back(quad);
	objects.push_back(odd);

	for(int i = 2; i < cubePositions.size(); i++)
	{
		glm::vec3 pos = cubePositions[i];
		RenderObject *ro = renderer->cloneObject(odd->rObjetID);

		ro->setPosition(pos);
		objects.push_back(ro);
	}

	while(!glfwWindowShouldClose(window))
	{
		//input
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		//change

		double sinTime = glm::sin(currentFrame);




		for(int i = 0; i < objects.size(); i++)
		{
			glm::vec3 scale = glm::vec3(1.0f);
			RenderObject *ro = objects[i];

			if(i % 4 == 0)
			{
				sinTime = glm::sin(currentFrame * 2);
			}
			else if(i % 2 == 0)
			{

				scale.y = 2.0f + sinTime;
				scale.x = 2.0f - sinTime;
				sinTime = 0;
			}
			if(i % 3 == 0)
			{
				sinTime *= -1.0;
			}
			ro->setRotation(glm::vec3(0.0f, 0.0, glm::radians(sinTime * 180)));
			ro->setPosition(ro->getPosition() + glm::vec3(0.0f, sinTime * deltaTime, 0.0));
			scale.x += sinTime * 0.5f;
			ro->setScale(scale);
		}

		//draw

		renderer->draw();

		//swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	delete shader;
	delete textures[0];
	delete textures[1];
	delete renderer;
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		texOffsety -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		texOffsety += 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		texOffsetx -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		texOffsetx += 0.05f;
	}

	const float cameraSpeed = 2.5 * deltaTime;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * camera->getCameraDir();
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * camera->getCameraDir();
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= camera->getCameraRight() * cameraSpeed;
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += camera->getCameraRight() * cameraSpeed;

	camera->setCameraPos(cameraPos);

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if(firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera->setYaw(camera->getYaw() + xoffset);
	camera->setPitch(camera->getPitch() + yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera->setFOV(camera->getFOV() - (float)yoffset);
}