// test shader for openGL 3.3 in glsl
// test shader for openGL 3.3 in glsl
#version 330 core

layout (location = 0) in vec3 aPos;       // base square vertex
layout (location = 1) in vec2 aSpawnPos;  // particle current position (not spawn position)
layout (location = 2) in float aSpawnTime; // time when particle spawned

out vec3 vertexColor;

uniform float size;        // size of each particle
uniform float fallspeed;   // falling speed (not used anymore - physics handled in CPU)
uniform float time;        // global time
uniform float cellWidth;   // width of one grid cell
uniform float cellHeight;  // height of one grid cell

void main()
{
    // Use the current particle position directly (no physics calculation here)
    vec3 pos = aPos * size + vec3(aSpawnPos, 0.0);
    
    gl_Position = vec4(pos, 1.0);

    // Color based on particle position with some time-based variation
    float age = time - aSpawnTime;
    vertexColor = vec3(
        aSpawnPos.x * 0.5 + 0.5, 
        aSpawnPos.y * 0.5 + 0.5, 
        0.5 + 0.3 * sin(age * 2.0)
    );
}