#pragma once

#include <memory>
#include <queue>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

class Renderer final {
  public:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void DisableLighting();
    void EnableLighting();
    void DrawLine(const glm::vec4& start, const glm::vec4& end);
    void SetColor(const float& r, const float& g, const float& b);

  private:
    struct RenderCommand {
        virtual void Do() = 0;
    };

    std::queue<std::unique_ptr<RenderCommand>> m_commands;
};