
#include <stdio.h>

#include <g2d/g2d.h>

#include <check.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int
main()
{
	GLFWwindow* window;
	GLint width = 800;
	GLint height = 600;
	GLint true_width, true_height;

	if (!glfwInit()) {
		fprintf(stderr, "GLFW window could not be initialized\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "Game", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return -1;
	}

    glfwGetFramebufferSize(window, &true_width, &true_height);

	glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, true_width, true_height);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}

