#include "MainWindow.h"

#include <chrono>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <Renderer.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/compatibility.hpp"

MainWindow::MainWindow()
    : lightRotation(0.0f), cameraLightEnabled(true), cameraKeyPressed(false), clicked(false), mouseDown(false),
      showRenderStats(false), showRenderStatsKeyPressed(false), picking(false) {
    std::cout << "Beweeg de joints van het skelet door te klikken en slepen met de muis " << std::endl;

    initGLFW();
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, resize);

    initGL();
    glewInit();
    camera = new Camera(window);
    MainWindow::resize(window, 640, 480);

    draw();

    cleanupModels();
    glfwDestroyWindow(window);
}

MainWindow::~MainWindow() {
    delete camera;
    std::cout << "GLFW window cleaned up" << std::endl;
    glfwTerminate();
}

void MainWindow::initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    initLighting();
}

void MainWindow::resize(GLFWwindow* window, int width, int height) {
    glViewport(0.0f, 0.0f, width, height);

    MainWindow::pMatrix = glm::perspective(90.0f, (float)width / (float)height, clipNear, clipFar);
}

void MainWindow::initLighting() {
    const GLfloat blue[] = {.3f, .3f, 1.f, 1.f};
    const GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat orange[] = {1.0f, 0.498f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, blue);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, orange);

    // 	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, red);
}

void MainWindow::handleMovable(glm::mat4 vpMatrix) {
    if (currentMovable == 0)
        return;

    dragging = true;

    int screenSize[2];
    glfwGetWindowSize(window, &screenSize[0], &screenSize[1]);

    float mousePosNormalized[] = {mousePos[0] / screenSize[0] * 2.0f - 1.0f,
                                  (mousePos[1] / screenSize[1] * 2.0f - 1.0f) * -1.0f};

    glm::mat4 const glm_inverse_mvpm = glm::inverse(vpMatrix);

    GLfloat depth[1];
    glReadPixels(mousePos[0], screenSize[1] - mousePos[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, depth);

    MoveHandler::setDepth(depth[0] * 2.0f - 1.0f);

    if (MoveHandler::getDepth() == 1.0f) {
        unregisterMovable();
        return;
    }

    glm::vec4 mouseVecScreen = glm::vec4(mousePosNormalized[0], mousePosNormalized[1], MoveHandler::getDepth(), 1.0f);

    glm::vec4 mouseVecWorld = glm_inverse_mvpm * mouseVecScreen;
    glm::vec4 objectLocation = vpMatrix * mouseVecWorld;
    objectLocation /= objectLocation.w;

    dotLocation = glm::vec3(objectLocation);
    currentMovable->move(glm::vec3(objectLocation));
}

void MainWindow::handleKeys() {
    mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT == GLFW_PRESS);

    glfwGetCursorPos(window, &mousePos[0], &mousePos[1]);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !cameraKeyPressed) {
        cameraLightEnabled = !cameraLightEnabled;
        cameraKeyPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_T) != GLFW_PRESS && cameraKeyPressed)
        cameraKeyPressed = false;

    if (cameraLightEnabled)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !showRenderStatsKeyPressed) {
        showRenderStats = !showRenderStats;
        showRenderStatsKeyPressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_F) != GLFW_PRESS && showRenderStatsKeyPressed)
        showRenderStatsKeyPressed = false;
}

unsigned long MainWindow::getCurrentTimeNanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
}

void MainWindow::draw() {
    initModels();

    unsigned long startTime = getCurrentTimeNanos();

    while (!glfwWindowShouldClose(window)) {
        deltaTime = (getCurrentTimeNanos() - startTime) / 1000000.0f;
        startTime = getCurrentTimeNanos();

        dragging = false;
        handleKeys();

        if (showRenderStats) {
            std::cout << deltaTime << " ms per frame" << std::endl;
        }

        glm::mat4 vpMatrix = camera->update(deltaTime, pMatrix);

        handleMovable(vpMatrix);

        if (mouseDown && !picking) {
            picking = true;
            glm::mat4 pickMatrix = Picking::pickObject(mousePos, pMatrix);
            render(pickMatrix * vpMatrix);
            Picking::selectObject();
        } else if (!mouseDown && picking) {
            picking = false;
            unregisterMovable();
        }

        render(vpMatrix);

        glfwPollEvents();
    }
}

void MainWindow::render(glm::mat4 vpMatrix) {
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderer renderer;

    if (dragging) {
        glDisable(GL_LIGHTING);
        glPointSize(10.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex3fv(&dotLocation[0]);
        glEnd();
        glEnable(GL_LIGHTING);
    }

    skeleton->setTransformMatrix(vpMatrix);
    skeleton->draw(renderer);

    glFlush();
    glfwSwapBuffers(window);
}

void MainWindow::initGLFW() {
    if (!glfwInit()) {
        std::cout << "Error initializing GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(640, 480, "Inverse kinematics", NULL, NULL);

    if (window == NULL) {
        std::cout << "Error creating window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::cout << "GLFW window created" << std::endl;
}

void MainWindow::initModels() { skeleton = new CSkeleton("skeleton.skl", this); }

void MainWindow::cleanupModels() { delete skeleton; }

const double MainWindow::getDeltaTime() { return deltaTime; }

double MainWindow::deltaTime = 0.0f;

const GLfloat MainWindow::shininess[] = {120.0};
const GLfloat MainWindow::zero[] = {0.0};
const GLfloat MainWindow::white[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat MainWindow::black[] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat MainWindow::light0_position[] = {3.0f, 1.0f, 0.0f, 1.0f};
const GLfloat MainWindow::light1_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
const float MainWindow::clipNear = 0.1f;
const float MainWindow::clipFar = 250.0f;
glm::mat4 MainWindow::pMatrix(1.0f);