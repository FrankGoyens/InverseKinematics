#ifndef DISPLAYLISTS_H
#define DISPLAYLISTS_H

#ifdef _WIN32
#include <windows.h>
#endif

#include "glm/mat4x4.hpp"
#include <GL/glew.h>

class DisplayList {
  private:
    GLuint m_currentIndex;

  public:
    ~DisplayList();

    DisplayList();
    void startDisplayList();
    void endDisplayList();
    void drawList(glm::mat4 vpMatrix);
};

#endif