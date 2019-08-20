#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
//#include <GL/gl.h>
#include "util.h"
#include "geometry.h"

// TODO change c arrays to std::vectors

static bool checkGlError(const std::string &place, const std::string &glCall) {
  bool allGood = true;
  GLenum error;
  while((error = glGetError()) != GL_NO_ERROR) {
    printf("OpenGl error at %s, %s: %s\n", gluErrorString(error), place.c_str(), glCall.c_str());
    allGood = false;
  }
  return allGood;
}

static void glSetup() {
  glewInit();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); 
  //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);


  if(glewIsSupported("GL_VERSION_4_5")) {
    printf("Glew version 4.5 supported\n");
  }
  else {
    printf("Glew version 4.5 not supported.\n");
  }
  printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}


static void glSetup(SDLInterface *sdlInterface) {
  glewInit();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); 
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, sdlInterface->screenW, sdlInterface->screenH, 0, -100, 100);
  glMatrixMode(GL_MODELVIEW);

/*
  glViewport(0.f, 0.f, sdlInterface->screenW, sdlInterface->screenH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0.0, sdlInterface->screenW, sdlInterface->screenH, 0.0, 1.0, -1.0 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glClearColor( 0.f, 0.f, 0.f, 1.f );
*/
  //glEnable( GL_TEXTURE_2D );

  GLenum error = glGetError();
  if(error != GL_NO_ERROR) {
    printf("Error at glSetup: %s\n", gluErrorString(error));
  }


  if(glewIsSupported("GL_VERSION_4_5")) {
    printf("Glew version 4.5 supported\n");
  }
  else {
    printf("Glew version 4.5 not supported.\n");
  }
  printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}


struct GlShader
{
  GLuint program;
  bool readyToUse = false;
  std::string nameFrag, nameVert;

  GlShader() {}

  GlShader(const char *vertexShaderSrc, const char *fragmentShaderSrc) {
    create(vertexShaderSrc, fragmentShaderSrc);
  }

  ~GlShader() {
    remove();
  }

  GLuint loadShader(const char *srcFilename, int shaderType) {
    GLuint shader = glCreateShader(shaderType);
    // GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
    TextFile srcFile(srcFilename);
    if(srcFile.status == File::OPEN_FAILED) {
      return -1;
    }

    int numStrings = 1;
    glShaderSource(shader, numStrings, &srcFile.contentCstr, NULL);

    glCompileShader(shader);
    int ret;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
    if(ret == GL_FALSE) {
      printf("Failed to compile shader %s\n", srcFilename);
      int infoLogLength, infoLogActualLength;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
      char *infoLog = new char[infoLogLength];
      glGetShaderInfoLog(shader, infoLogLength, &infoLogActualLength, infoLog);
      printf("%s\n", infoLog);
      delete infoLog;
      return -1;
    }

    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
      printf("Error at GlShader.loadShader(%s, %d): %s\n", srcFilename, shaderType, gluErrorString(error));
    }

    printf("%s compiled succesfully!\n", srcFilename);
    return shader;
  }

  void create(const char *vertexShaderSrc, const char *fragmentShaderSrc) {
    GLuint vertexShader = loadShader(vertexShaderSrc, GL_VERTEX_SHADER);
    if(vertexShader == -1) {
      return;
    }
    GLuint fragmentShader = loadShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    if(fragmentShader == -1) {
      return;
    }
    nameFrag = extractFilename(fragmentShaderSrc);
    nameVert = extractFilename(vertexShaderSrc);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    //printf("Vertex shader attached!\n");
    glAttachShader(program, fragmentShader);
    //printf("Fragment shader attached!\n");
    //glDeleteShader(shader);
    glLinkProgram(program);

    int ret;
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if(ret == GL_FALSE) {
      printf("Failed to link shader program ... \n");
      int infoLogLength, infoLogActualLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
      char *infoLog = new char[infoLogLength];
      glGetProgramInfoLog(program, infoLogLength, &infoLogActualLength, infoLog);
      printf("%s\n", infoLog);
      delete infoLog;
      return;
    }

    //printf("Shader program linked!\n");

    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
      printf("Error at GlShader.create: %s\n", gluErrorString(error));
    }

    readyToUse = true;
  }

  void activate() {
    glUseProgram(program);
  }
  void deActivate() {
    glUseProgram(0);
  }
  void remove() {
    glDeleteProgram(program);
  }

  void setUniformMatrix4fv(const std::string &identifier, int size, const double *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniformMatrix4f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    std::vector<float> arr(16*size);
    for(int i=0; i<arr.size(); i++) {
      arr[i] = array[i];
    }
    glUniformMatrix4fv(loc, size, false, arr.data());
    checkGlError("GlShader.setUniformMatrix4f()", "glUniformMatrix4fv; "+identifier+", "+nameFrag+", "+nameVert);
  }


  void setUniform1i(const std::string &identifier, int a) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1i()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1i(loc, a);
    checkGlError("GlShader.setUniform1i()", "glUniform1i; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2i(const std::string &identifier, int a, int b) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform2i()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform2i(loc, a, b);
    checkGlError("GlShader.setUniform2i()", "glUniform2i; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform1iv(const std::string &identifier, int size, const int *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1vi()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1iv(loc, size, array);
    checkGlError("GlShader.setUniform1vi()", "glUniform1iv; "+identifier+", "+nameFrag+", "+nameVert);
  }

  void setUniform1f(const std::string &identifier, float a) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1f(loc, a);
    checkGlError("GlShader.setUniform1f()", "glUniform1f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2f(const std::string &identifier, float a, float b) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform2f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform2f(loc, a, b);
    checkGlError("GlShader.setUniform2f()", "glUniform2f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2f(const std::string &identifier, Vec2d v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform2f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform2f(loc, v.x, v.y);
    checkGlError("GlShader.setUniform2f()", "glUniform2f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform1fv(const std::string &identifier, int size, const float *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1fv()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1fv(loc, size, array);
    checkGlError("GlShader.setUniform1fv()", "glUniform1fv; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform1fv(const std::string &identifier, int size, const double *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1fv()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    std::vector<float> arr(size);
    for(int i=0; i<size; i++) {
      arr[i] = array[i];
    }
    glUniform1fv(loc, size, arr.data());
    checkGlError("GlShader.setUniform1fv()", "glUniform1fv; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2fv(const std::string &identifier, int size, const std::vector<Vec2d> &v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform2fv()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    std::vector<float> arr(size*2);
    for(int i=0; i<size; i++) {
      arr[2*i] = (float)v[i].x;
      arr[2*i+1] = (float)v[i].y;
    }
    glUniform2fv(loc, size, arr.data());
    checkGlError("GlShader.setUniform2fv()", "glUniform2fv; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2fv(const std::string &identifier, int size, const Vec2d *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1fv()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    std::vector<float> arr(size*2);
    for(int i=0; i<size; i++) {
      arr[i*2] = array[i].x;
      arr[i*2+1] = array[i].y;
    }
    glUniform2fv(loc, size, arr.data());
    checkGlError("GlShader.setUniform1fv()", "glUniform1fv; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform1d(const std::string &identifier, double a) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1d()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1d(loc, a);
    checkGlError("GlShader.setUniform1d()", "glUniform1d; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform2d(const std::string &identifier, double a, double b) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform2d()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform2d(loc, a, b);
    checkGlError("GlShader.setUniform2d()", "glUniform2d; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform4d(const std::string &identifier, const Vec4d &v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform4d()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform4d(loc, v.x, v.y, v.z, v.w);
    checkGlError("GlShader.setUniform4d()", "glUniform4d; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform3f(const std::string &identifier, const Vec3d &v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform3f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform3f(loc, v.x, v.y, v.z);
    checkGlError("GlShader.setUniform3f()", "glUniform3f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform3i(const std::string &identifier, const Vec3i &v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform3i()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform3i(loc, v.x, v.y, v.z);
    checkGlError("GlShader.setUniform3i()", "glUniform3i; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform3f(const std::string &identifier, float a, float b, float c) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform3f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform3f(loc, a, b, c);
    checkGlError("GlShader.setUniform3f()", "glUniform3f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform4f(const std::string &identifier, const Vec4d &v) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform4f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
    checkGlError("GlShader.setUniform4f()", "glUniform4f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform4f(const std::string &identifier, float a, float b, float c, float d) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform4f()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform4f(loc, a, b, c, d);
    checkGlError("GlShader.setUniform4f()", "glUniform4f; "+identifier+", "+nameFrag+", "+nameVert);
  }
  void setUniform1dv(const std::string &identifier, int size, const double *array) {
    GLint loc = glGetUniformLocation(this->program, identifier.c_str());
    checkGlError("GlShader.setUniform1dv()", "glGetUniformLocation; "+identifier+", "+nameFrag+", "+nameVert);
    glUniform1dv(loc, size, array);
    checkGlError("GlShader.setUniform1dv()", "glUniform1dv; "+identifier+", "+nameFrag+", "+nameVert);
  }

};


struct GlScene
{
  GLuint vao;

  void create() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }
  void cleanup() {
    glDeleteVertexArrays(1, &vao);
  }

};

struct Quad
{
  float xy;
  float w = 100, h = 100;
  GLuint vbo;

  Quad() {}

  Quad(float w, float h) {
    create(w, h);
  }


  void create(float w, float h) {
    /*GLfloat bufferData[16] = {
      -w*0.5f, h*0.5f, 0.0f,
      -w*0.5f, -h*0.5f, 0.0f,
      w*0.5f, h*0.5f, 0.0f,
      w*0.5f, -h*0.5f, 0.0f,
    };*/
    GLfloat vertices[12] = {
      -w, h, 0.0f,
      -w, -h, 0.0f,
      w, h, 0.0f,
      w, -h, 0.0f,
    };
    /*GLfloat vertices[12] = {
      -w*0.5f, h*0.5f, 0.0f,
      -w*0.5f, -h*0.5f, 0.0f,
      w*0.5f, h*0.5f, 0.0f,
      w*0.5f, -h*0.5f, 0.0f
    };

    GLfloat texCoords[8] = {
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f
    };

    GLfloat normals[12] = {
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f
    };*/
    /*GLfloat data[12+12+8] = {
      -w*0.5f, h*0.5f, 0.0f,
      -w*0.5f, -h*0.5f, 0.0f,
      w*0.5f, h*0.5f, 0.0f,
      w*0.5f, -h*0.5f, 0.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f
    };*/

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    //glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
    //glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), sizeof(texCoords), texCoords);


    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glEnableVertexAttribArray(0);
  }

  void render(GlShader &shader) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int vertexLocation = 0; // glGetAttribLocation(shader.program, "vertex");
    int normalLocation = 1; // glGetAttribLocation(shader.program, "normal");
    int texCoordLocation = 2; // glGetAttribLocation(shader.program, "texCoord");
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    /*glEnableVertexAttribArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*12));

    glEnableVertexAttribArray(texCoordLocation);
    glVertexAttribPointer(texCoordLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*24));*/

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
  }

  void render() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glDisableVertexAttribArray(0);
  }

  void cleanup() {
    glDeleteBuffers(1, &vbo);
  }
};

static GLfloat quadDataTopRightUV[12+12+8+16] = {
  /*-w*0.5f/w, h*0.5f/w, 0.0f,
  -w*0.5f/w, -h*0.5f/w, 0.0f,
  w*0.5f/w, h*0.5f/w, 0.0f,
  w*0.5f/w, -h*0.5f/w, 0.0f,*/
  -0.5f, 0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
  0.5f, 0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 1.0f,
  0.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f,
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.7f, 0.7f, 0.7f, 1.0f,
};

static GLfloat quadDataBottomRightUV[12+12+8+16] = {
  /*-w*0.5f/w, h*0.5f/w, 0.0f,
  -w*0.5f/w, -h*0.5f/w, 0.0f,
  w*0.5f/w, h*0.5f/w, 0.0f,
  w*0.5f/w, -h*0.5f/w, 0.0f,*/
  -0.5f, 0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
  0.5f, 0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.7f, 0.7f, 0.7f, 1.0f,
};


struct Quadx
{
  float w = 100, h = 100;
  GLuint vao, vbo;
  bool useBottomRightUV = false;

  Quadx() {}

  Quadx(float w, float h, bool useBottomRightUV = true) {
    create(w, h, useBottomRightUV);
  }

  void setSize(float w, float h) {
    this->w = w;
    this->h = h;
  }

  void create(float w, float h, bool useBottomRightUV = true) {
    this->w = w;
    this->h = h;
    this->useBottomRightUV = useBottomRightUV;

    glGenVertexArrays(1, &vao);
    checkGlError("Quadx.create()", "glGenVertexArrays()");

    glBindVertexArray(vao);
    checkGlError("Quadx.create()", "glBindVertexArray()");

    glGenBuffers(1, &vbo);
    checkGlError("Quadx.create()", "glGenBuffers()");
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    checkGlError("Quadx.create()", "glBindBuffer()");

    if(useBottomRightUV) {
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadDataBottomRightUV), quadDataBottomRightUV, GL_STATIC_DRAW);
    }
    else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadDataTopRightUV), quadDataTopRightUV, GL_STATIC_DRAW);
    }
    checkGlError("Quadx.create()", "glBufferData()");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    checkGlError("Quadx.create()", "glVertexAttribPointer(0, ...)");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*12));
    checkGlError("Quadx.create()", "glVertexAttribPointer(1, ...)");
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*24));
    checkGlError("Quadx.create()", "glVertexAttribPointer(2, ...)");
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*(24+8)));
    checkGlError("Quadx.create()", "glVertexAttribPointer(3, ...)");
    glEnableVertexAttribArray(0);
    checkGlError("Quadx.create()", "glEnableVertexAttribArray(0)");
    glEnableVertexAttribArray(1);
    checkGlError("Quadx.create()", "glEnableVertexAttribArray(1)");
    glEnableVertexAttribArray(2);
    checkGlError("Quadx.create()", "glEnableVertexAttribArray(2)");
    glEnableVertexAttribArray(3);
    checkGlError("Quadx.create()", "glEnableVertexAttribArray(3)");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGlError("Quadx.create()", "glBindBuffer(0)");
    glBindVertexArray(0);
    checkGlError("Quadx.create()", "glBindVertexArray(0)");

    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
      printf("Error at Quadx.create (%f, %f): %s\n", w, h, gluErrorString(error));
    }
  }

  void render() {
    render(w*0.5, h*0.5, 0.0, 0.0, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y) {
    render(x, y, 0.0, 0.0, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y, const double &rotation) {
    render(x, y, 0.0, rotation, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y, const double &rotation, const double &scale) {
    render(x, y, 0.0, rotation, scale, scale, scale);
  }
  inline void render(const double &x, const double &y, const double &rotation, const double &scaleX, const double &scaleY) {
    render(x, y, 0.0, rotation, scaleX, scaleY, 1.0);
  }

  void render(const double &x, const double &y, const double &z, const double &rotation, const double &scaleX, const double &scaleY, const double &scaleZ) {
    glBindVertexArray(vao);
    checkGlError("Quadx.render()", "glBindVertexArray(vao)");

    glPushMatrix();
    glTranslated(x, y, z);
    glRotatef(rotation*180.0/PI, 0, 0, 1);
    glScalef(scaleX, scaleY, scaleZ);
    glScalef(w, h, 1.0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("Quadx.render()", "glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)");

    glPopMatrix();

    glBindVertexArray(0);
    checkGlError("Quadx.render()", "glBindVertexArray(0)");
  }

  void cleanup() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
  }
};


static GLfloat triangleDataTopRightUV[9+9+6+12] = {
  -0.5f, 0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
  0.5f, 0.5f, 0.0f,
//  0.5f, -0.5f, 0.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
//  0.0f, 0.0f, -1.0f,
  0.0f, 1.0f,
  0.0f, 0.0f,
  1.0f, 1.0f,
//  1.0f, 0.0f,
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
//  0.7f, 0.7f, 0.7f, 1.0f,
};

static GLfloat triangleDataBottomRightUV[9+9+6+12] = {
  -0.5f, 0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
  0.5f, 0.5f, 0.0f,
//  0.5f, -0.5f, 0.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f, -1.0f,
//  0.0f, 0.0f, -1.0f,
  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 0.0f,
//  1.0f, 1.0f,
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
//  0.7f, 0.7f, 0.7f, 1.0f,
};

struct Trianglex
{
  Vec2d a, b, c;
  GLuint vao, vbo;
  bool useBottomRightUV = false;

  Trianglex() {}

  inline void set(const Vec2d &a, const Vec2d &b, const Vec2d &c) {
    set(a.x, a.y, b.x, b.y, c.x, c.y);
  }

  inline void set(const Triangle &t) {
    set(t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y);
  }

  void set(const double &x0, const double &y0, const double &x1, const double &y1, const double &x2, const double &y2) {
    this->a.set(x0, y0);
    this->b.set(x1, y1);
    this->c.set(x2, y2);
    //glBindVertexArray(vao);
    //checkGlError("Trianglex.set()", "glBindVertexArray(vao)");
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //checkGlError("set.create()", "glBindBuffer(vbo)");
    GLfloat vertices[9] = {(GLfloat)a.x, (GLfloat)a.y, 0, (GLfloat)b.x, (GLfloat)b.y, 0, (GLfloat)c.x, (GLfloat)c.y, 0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    //checkGlError("Trianglex.set()", "glBufferData()");
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //checkGlError("set.create()", "glBindBuffer(0)");
    //glBindVertexArray(0);
    //checkGlError("Trianglex.set()", "glBindVertexArray(0)");
  }

  void create(const Vec2d &a, const Vec2d &b, const Vec2d &c, bool useBottomRightUV = true) {
    create(a.x, a.y, b.x, b.y, c.x, c.y, useBottomRightUV);
  }

  void create(const Triangle &t , bool useBottomRightUV = true) {
    create(t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y, useBottomRightUV);
  }

  void create(const double &x0, const double &y0, const double &x1, const double &y1, const double &x2, const double &y2, bool useBottomRightUV = true) {
    this->a.set(x0, y0);
    this->b.set(x1, y1);
    this->c.set(x2, y2);

    this->useBottomRightUV = useBottomRightUV;

    glGenVertexArrays(1, &vao);
    checkGlError("Trianglex.create()", "glGenVertexArrays()");

    glBindVertexArray(vao);
    checkGlError("Trianglex.create()", "glBindVertexArray()");

    glGenBuffers(1, &vbo);
    checkGlError("Trianglex.create()", "glGenBuffers()");
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    checkGlError("Trianglex.create()", "glBindBuffer()");

    if(useBottomRightUV) {
      glBufferData(GL_ARRAY_BUFFER, sizeof(triangleDataBottomRightUV), triangleDataBottomRightUV, GL_DYNAMIC_DRAW);
    }
    else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(triangleDataTopRightUV), triangleDataTopRightUV, GL_DYNAMIC_DRAW);
    }
    checkGlError("Trianglex.create()", "glBufferData()");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    checkGlError("Trianglex.create()", "glVertexAttribPointer(0, ...)");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*9));
    checkGlError("Trianglex.create()", "glVertexAttribPointer(1, ...)");
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*18));
    checkGlError("Trianglex.create()", "glVertexAttribPointer(2, ...)");
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GL_FLOAT)*(18+6)));
    checkGlError("Trianglex.create()", "glVertexAttribPointer(3, ...)");
    glEnableVertexAttribArray(0);
    checkGlError("Trianglex.create()", "glEnableVertexAttribArray(0)");
    glEnableVertexAttribArray(1);
    checkGlError("Trianglex.create()", "glEnableVertexAttribArray(1)");
    glEnableVertexAttribArray(2);
    checkGlError("Trianglex.create()", "glEnableVertexAttribArray(2)");
    glEnableVertexAttribArray(3);
    checkGlError("Trianglex.create()", "glEnableVertexAttribArray(3)");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGlError("Trianglex.create()", "glBindBuffer(0)");
    glBindVertexArray(0);
    checkGlError("Trianglex.create()", "glBindVertexArray(0)");

    //set(a, b, c);
  }

  void render() {
    render(0, 0, 0.0, 0.0, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y) {
    render(x, y, 0.0, 0.0, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y, const double &rotation) {
    render(x, y, 0.0, rotation, 1.0, 1.0, 1.0);
  }

  inline void render(const double &x, const double &y, const double &rotation, const double &scale) {
    render(x, y, 0.0, rotation, scale, scale, scale);
  }
  inline void render(const double &x, const double &y, const double &rotation, const double &scaleX, const double &scaleY) {
    render(x, y, 0.0, rotation, scaleX, scaleY, 1.0);
  }

  void render(const double &x, const double &y, const double &z, const double &rotation, const double &scaleX, const double &scaleY, const double &scaleZ) {
    glBindVertexArray(vao);
    checkGlError("Trianglex.render()", "glBindVertexArray(vao)");

    glPushMatrix();
    glTranslated(x, y, z);
    glRotatef(rotation*180.0/PI, 0, 0, 1);
    glScalef(scaleX, scaleY, scaleZ);
    //glScalef(w, h, 1.0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("Trianglex.render()", "glDrawArrays(GL_TRIANGLE, 0, 3)");

    glPopMatrix();

    glBindVertexArray(0);
    checkGlError("Trianglex.render()", "glBindVertexArray(0)");
  }

  void cleanup() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
  }
};


/*struct Triangle
{
  float w = 100, h = 100;
  GLuint vbo;
  GLuint shaderProgram = 0;

  Triangle() {}

  Triangle(float w, float h, GlShader &shader) {
    create(w, h, shader);
  }


  void create(float w, float h, GlShader &shader) {
    shaderProgram = shader.program;
    GLfloat bufferData[16] = {
      w*0.5f, h*0.5f, 0.0f,
      w*0.5f, -h*0.5f, 0.0f,
      -w*0.5f, -h*0.5f, 0.0f,
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glEnableVertexAttribArray(0);
  }

  void render() {
    if(shaderProgram != 0) {
      glUseProgram(shaderProgram);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
  }

  void cleanup() {
    glDeleteBuffers(1, &vbo);
  }
};*/
