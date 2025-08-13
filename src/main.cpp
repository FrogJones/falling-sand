#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

// ====================== Globals & Constants ======================
const unsigned int SCR_WIDTH  = 1000;
const unsigned int SCR_HEIGHT = 1000;
const unsigned int MAX_PARTICLES = 1000;

// ====================== Callbacks ======================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// ====================== Particle Struct ======================
struct Particle {
    float x, y;
    float spawnTime;
    bool active;
};

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mouse Particle Spawn", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    
    // Square vertices
    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f
    };
    
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    Shader shader("test.vert", "test.frag");

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Instance buffer for spawn positions
    unsigned int instanceVBO1;
    glGenBuffers(1, &instanceVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO1);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // Update per instance

    // Instance buffer for spawn times
    unsigned int instanceVBO2;
    glGenBuffers(1, &instanceVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // Update per instance

    glBindVertexArray(0);

    std::vector<Particle> particles;

    // ====================== Render Loop ======================
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Get mouse position in NDC
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        float ndcX = 2.0f * (mouseX / SCR_WIDTH) - 1.0f;
        float ndcY = 1.0f - 2.0f * (mouseY / SCR_HEIGHT);

        // Spawn a new particle at mouse
        if (particles.size() < MAX_PARTICLES) {
            particles.push_back({ndcX, ndcY, (float)glfwGetTime(), true});
        }

        // Fill instance arrays
        std::vector<float> spawnPositions(MAX_PARTICLES * 2, 0.0f);
        std::vector<float> spawnTimes(MAX_PARTICLES, 0.0f);
        unsigned int count = 0;
        for (auto &p : particles) {
            if (p.active) {
                spawnPositions[count * 2 + 0] = p.x;
                spawnPositions[count * 2 + 1] = p.y;
                spawnTimes[count] = p.spawnTime;
                count++;
            }
        }

        // Update VBOs
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO1);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * 2 * sizeof(float), spawnPositions.data());
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(float), spawnTimes.data());

        // Draw particles
        shader.use();
        glUniform1f(glGetUniformLocation(shader.ID, "size"), 0.01f);
        glUniform1f(glGetUniformLocation(shader.ID, "fallspeed"), 0.5f);
        glUniform1f(glGetUniformLocation(shader.ID, "time"), (float)glfwGetTime());

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << "Mouse Particle Spawn completed successfully!\n";
    return 0;
}
