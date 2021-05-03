#include <Renderer.h>

#include <GL/glew.h>
#include <GL/glu.h>

void Renderer::DisableLighting() { glDisable(GL_LIGHTING); }

void Renderer::EnableLighting() { glEnable(GL_LIGHTING); }

void Renderer::DrawLine(const glm::vec4& start, const glm::vec4& end) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex4fv(&start[0]);
    glVertex4fv(&end[0]);
    glEnd();
}

void Renderer::SetColor(const float& r, const float& g, const float& b) { glColor3f(r, g, b); }
