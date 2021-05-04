#pragma once

#include <memory>
#include <queue>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace Ogre {
class Root;
class SceneManager;
} // namespace Ogre

class Renderer final {
  public:
    Renderer(Ogre::Root&, Ogre::SceneManager&);
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void DisableLighting();
    void EnableLighting();

    void DrawLine(const glm::vec4& start, const glm::vec4& end);
    void Sphere(const glm::vec4& position);

    void SetColor(const float& r, const float& g, const float& b);

  private:
    Ogre::Root* m_ogreRoot;
    Ogre::SceneManager* m_sceneManager;
};