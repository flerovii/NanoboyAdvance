/*
 * Copyright (C) 2021 fleroviux
 *
 * Licensed under GPLv3 or any later version.
 * Refer to the included LICENSE file.
 */

#pragma once

#include <nba/device/video_device.hpp>
#include <GL/glew.h>
#include <string>

namespace nba {

struct OGLVideoDevice : VideoDevice {
  ~OGLVideoDevice() override;

  void Initialize();
  void SetViewport(int x, int y, int width, int height);
  void SetDefaultFBO(GLuint fbo);
  void Draw(u32* buffer) override;
private:
  void LoadShader(std::string const& path);
  auto CompileShader(GLenum type, const char* source) -> GLuint;

  int view_x = 0;
  int view_y = 0;
  int view_width  = 1;
  int view_height = 1;
  GLuint default_fbo = 0;

  GLuint quad_vao;
  GLuint quad_vbo;
  GLuint program_a;
  GLuint program_b;
  GLuint program_out;
  GLuint fbo;
  GLuint texture[4];
};

} // namespace nba