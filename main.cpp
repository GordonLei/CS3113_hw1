#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram tex_program, non_tex_program;
glm::mat4 viewMatrix, slimeMatrix, boxMatrix, triangleMatrix, projectionMatrix;

float slime_x = -6.0f;
float box_x = -5.0f;

GLuint slimeTextureID, boxTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    tex_program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    non_tex_program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    slimeTextureID = LoadTexture("slime.png");
    boxTextureID = LoadTexture("not_snake.png");
    
    
    viewMatrix = glm::mat4(1.0f);
    slimeMatrix = glm::mat4(1.0f);
    boxMatrix = glm::mat4(1.0f);
    triangleMatrix = glm::mat4(1.0f);
    //10 pixel width and 7.5 height.
    //so height is 75% of 10.
    //640 * 0.75 = 480
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    tex_program.SetProjectionMatrix(projectionMatrix);
    tex_program.SetViewMatrix(viewMatrix);
    tex_program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    non_tex_program.SetProjectionMatrix(projectionMatrix);
    non_tex_program.SetViewMatrix(viewMatrix);
    non_tex_program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(tex_program.programID);
    glUseProgram(non_tex_program.programID);
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0;
float rotate_z = 0;

void update_slime(float deltaTime){
    slime_x += 1.0f * deltaTime;
    slimeMatrix = glm::mat4(1.0f);
    slimeMatrix = glm::translate(slimeMatrix, glm::vec3(slime_x, 0.0f, -.2f));
}

void update_box(float deltaTime){
    box_x += 1.1f * deltaTime;
    
    boxMatrix = glm::mat4(1.0f);
    boxMatrix = glm::translate(boxMatrix, glm::vec3(box_x, 0.0f, -.2f));
}

void update_triangle(float deltaTime){
    rotate_z += 45.0 * deltaTime;
    
}
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    update_box(deltaTime);
    update_slime(deltaTime);
    update_triangle(deltaTime);

}

void draw_slime(){
    slimeMatrix =glm::translate(slimeMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    
    tex_program.SetModelMatrix(slimeMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, slimeTextureID);
    
    glVertexAttribPointer(tex_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(tex_program.positionAttribute);
    
    glVertexAttribPointer(tex_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(tex_program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(tex_program.positionAttribute);
    glDisableVertexAttribArray(tex_program.texCoordAttribute);

}

void draw_box(){
    boxMatrix =glm::translate(boxMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    
    tex_program.SetModelMatrix(boxMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, boxTextureID);
    
    glVertexAttribPointer(tex_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(tex_program.positionAttribute);
    
    glVertexAttribPointer(tex_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(tex_program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(tex_program.positionAttribute);
    glDisableVertexAttribArray(tex_program.texCoordAttribute);
    
}

void draw_triangle(){
    
    //draw a triangle with (x,y) (x,y) (x,y)
    float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
    
    triangleMatrix = glm::mat4(1.0f);

    triangleMatrix = glm::rotate(triangleMatrix, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));

    non_tex_program.SetModelMatrix(triangleMatrix);
    
    
    
    glVertexAttribPointer(non_tex_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(non_tex_program.positionAttribute);
    //tell openGL to draw triangles with 3 points
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(non_tex_program.positionAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    draw_box();
    draw_slime();
    draw_triangle();
    
    SDL_GL_SwapWindow(displayWindow);
    
    
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
