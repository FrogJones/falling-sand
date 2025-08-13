#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include "shader.h"

// ====================== Globals & Constants ======================
const unsigned int SCR_WIDTH  = 1000;
const unsigned int SCR_HEIGHT = 1000;
const unsigned int MAX_PARTICLES = 100000;
const int GRID_SIZE = 300;
float cellWidth  = 2.0f / GRID_SIZE;
float cellHeight = 2.0f / GRID_SIZE;

// Mouse state tracking
bool mousePressed = false;
float lastSpawnTime = 0.0f;
const float SPAWN_RATE = 100.0f; // particles per second when holding mouse

// Random number generator for sliding direction
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> slideDir(0, 1); // 0 = left first, 1 = right first

// ====================== Callbacks ======================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Track mouse button state
    mousePressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

// ====================== Particle Struct ======================
struct Particle {
    float x, y;
    float spawnTime;
    float lastFallTime;
    bool active;
    bool settled;
    
    Particle() : x(0), y(0), spawnTime(0), lastFallTime(0), active(false), settled(false) {}
    Particle(float px, float py, float time) : x(px), y(py), spawnTime(time), lastFallTime(time), active(true), settled(false) {}
};

// Helper function to convert world coordinates to grid coordinates
void worldToGrid(float worldX, float worldY, int& gridX, int& gridY) {
    gridX = (int)((worldX + 1.0f) / 2.0f * GRID_SIZE);
    gridY = (int)((worldY + 1.0f) / 2.0f * GRID_SIZE);
    
    // Clamp to valid range
    gridX = std::max(0, std::min(GRID_SIZE - 1, gridX));
    gridY = std::max(0, std::min(GRID_SIZE - 1, gridY));
}

// Helper function to convert grid coordinates to world coordinates (center of cell)
void gridToWorld(int gridX, int gridY, float& worldX, float& worldY) {
    worldX = (gridX + 0.5f) * cellWidth - 1.0f;
    worldY = (gridY + 0.5f) * cellHeight - 1.0f;
}

// Check if a grid position is valid and empty
bool isValidAndEmpty(const std::vector<std::vector<bool>>& grid, int x, int y) {
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && !grid[x][y];
}

// Try to slide the particle left or right
bool trySlide(Particle& p, std::vector<std::vector<bool>>& grid, int currentGridX, int currentGridY) {
    // Randomly choose which direction to try first
    bool tryLeftFirst = slideDir(gen) == 0;
    
    for (int attempt = 0; attempt < 2; attempt++) {
        int slideX = currentGridX + (tryLeftFirst ? -1 : 1);
        
        // Check if we can slide to this position and then fall
        if (isValidAndEmpty(grid, slideX, currentGridY)) {
            // Check if there's space to fall diagonally
            if (isValidAndEmpty(grid, slideX, currentGridY - 1)) {
                // Clear current position
                grid[currentGridX][currentGridY] = false;
                
                // Move to slide position and fall
                gridToWorld(slideX, currentGridY - 1, p.x, p.y);
                grid[slideX][currentGridY - 1] = true;
                return true;
            }
        }
        
        // Try the other direction
        tryLeftFirst = !tryLeftFirst;
    }
    
    return false; // Couldn't slide either direction
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Falling Sand with Sliding", nullptr, nullptr);
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
    glVertexAttribDivisor(1, 1);

    // Instance buffer for spawn times
    unsigned int instanceVBO2;
    glGenBuffers(1, &instanceVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);

    // Initialize grid and particles
    std::vector<Particle> particles;
    std::vector<std::vector<bool>> grid(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));
    
    float lastTime = glfwGetTime();
    float fallSpeed = 100.0f; // Random fall speed between 20 and 50
    
    // ====================== Render & Update Loop ======================
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // ------------------- Spawn Particle While Mouse Held -------------------
        if (mousePressed && (currentTime - lastSpawnTime) >= (1.0f / SPAWN_RATE)) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            float ndcX = 2.0f * (mouseX / SCR_WIDTH) - 1.0f;
            float ndcY = 1.0f - 2.0f * (mouseY / SCR_HEIGHT);

            int mouseGridX, mouseGridY;
            worldToGrid(ndcX, ndcY, mouseGridX, mouseGridY);
            
            bool spawned = false;
            int searchRadius = 3;
            
            for (int dy = 0; dy <= searchRadius && !spawned; dy++) {
                for (int dx = -dy; dx <= dy && !spawned; dx++) {
                    int checkX = mouseGridX + dx;
                    int checkY = mouseGridY + dy;
                    
                    for (int yOffset : {0, -dy}) {
                        if (yOffset != 0 && dy == 0) continue;
                        int finalY = checkY + yOffset;
                        
                        if (particles.size() >= MAX_PARTICLES) {
                            spawned = true;
                            break;
                        }
                        
                        if (isValidAndEmpty(grid, checkX, finalY)) {
                            float spawnX, spawnY;
                            gridToWorld(checkX, finalY, spawnX, spawnY);
                            particles.emplace_back(spawnX, spawnY, currentTime);
                            grid[checkX][finalY] = true;
                            lastSpawnTime = currentTime;
                            spawned = true;
                            break;
                        }
                    }
                }
            }
        }

        // ------------------- Update Particles with Sliding Logic -------------------
        for (int i = particles.size() - 1; i >= 0; --i) {
            Particle& p = particles[i];
            if (!p.active || p.settled) continue;

            int currentGridX, currentGridY;
            worldToGrid(p.x, p.y, currentGridX, currentGridY);
            
            float timeSinceLastFall = currentTime - p.lastFallTime;
            float fallInterval = 1.0f / fallSpeed;
            
            if (timeSinceLastFall >= fallInterval) {
                // Try to fall straight down first
                if (isValidAndEmpty(grid, currentGridX, currentGridY - 1)) {
                    // Clear current position
                    grid[currentGridX][currentGridY] = false;
                    
                    // Move down one cell
                    currentGridY--;
                    gridToWorld(currentGridX, currentGridY, p.x, p.y);
                    grid[currentGridX][currentGridY] = true;
                    
                    p.lastFallTime = currentTime;
                } 
                // If can't fall straight down, try to slide
                else if (trySlide(p, grid, currentGridX, currentGridY)) {
                    p.lastFallTime = currentTime;
                } 
                // If can't slide either, particle has settled
                else {
                    p.settled = true;
                }
            }
        }

        // ------------------- Fill Instance Buffers -------------------
        std::vector<float> spawnPositions;
        std::vector<float> spawnTimes;
        
        for (const auto& p : particles) {
            if (p.active) {
                spawnPositions.push_back(p.x);
                spawnPositions.push_back(p.y);
                spawnTimes.push_back(p.spawnTime);
            }
        }
        
        unsigned int count = spawnTimes.size();

        if (count > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO1);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * 2 * sizeof(float), spawnPositions.data());
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(float), spawnTimes.data());
        }

        // ------------------- Draw Particles -------------------
        if (count > 0) {
            shader.use();
            glUniform1f(glGetUniformLocation(shader.ID, "size"), cellWidth);
            glUniform1f(glGetUniformLocation(shader.ID, "fallspeed"), std::max(0.1f, 0.0f));
            glUniform1f(glGetUniformLocation(shader.ID, "time"), currentTime);
            glUniform1f(glGetUniformLocation(shader.ID, "cellWidth"), cellWidth);
            glUniform1f(glGetUniformLocation(shader.ID, "cellHeight"), cellHeight);

            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, count);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << "Falling Sand with Sliding completed successfully!\n";
    return 0;
}