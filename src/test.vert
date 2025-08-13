// test shader for openGL 3.3 in glsl
#version 330 core

layout (location = 0) in vec3 aPos;       // base square vertex
layout (location = 1) in vec2 aSpawnPos;  // particle spawn position
layout (location = 2) in float aSpawnTime; // time when particle spawned

out vec3 vertexColor;

uniform float size;        // size of each particle
uniform float fallspeed;   // falling speed
uniform float time;        // global time

void main()
{
    // Compute particle age
    float age = time - aSpawnTime;

    // Particle position: start at spawnPos, fall down over time
    vec3 pos = aPos * size + vec3(aSpawnPos, 0.0);
    pos.y -= fallspeed * age;

    gl_Position = vec4(pos, 1.0);

    // Color based on spawn position (fixed per particle)
    vertexColor = vec3(aSpawnPos.x * 0.5 + 0.5, aSpawnPos.y * 0.5 + 0.5, 0.5);
}

