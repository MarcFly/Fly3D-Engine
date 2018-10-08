#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "GLEW/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "glmath/glmath.h"

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Render(bool use_texture);
	void OnResize(int width, int height);
	void ChangeVsync();
	bool CheckGLError();
	void RenderGrid();
	void SwapWireframe(bool active);

	void InitCheckTex();

public:
	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;

	// Options
	bool depth_test;
	bool cull_face;
	bool lighting;
	bool color_material;
	bool texture_2d;
	bool wireframe;
	bool faces;
	bool render_normals;
	float normal_length;

  // Checker Texture
	GLubyte checkTexture[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
  
};

#endif