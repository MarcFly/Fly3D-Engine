#ifndef _Res_Mesh_
#define _Res_Mesh_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

struct ResourceMaterial;
struct GUID;

struct ResourceMesh
{
	GLuint id_index = 0; // index in VRAM
	GLuint num_index = 0;
	GLuint* index = nullptr;

	GLuint id_vertex = 0; // unique vertex in VRAM
	GLuint num_vertex = 0;
	GLfloat* vertex = nullptr;

	GLuint id_uvs = 0;
	GLuint num_uvs = 0;
	GLfloat* tex_coords = nullptr;

	GLuint id_normal = 0;
	GLuint num_normal = 0;
	GLfloat* normal = nullptr;

	ResourceMaterial* mat = nullptr;

	GLuint num_faces = 0;

	GUID* UID = nullptr;
};

class MetaMesh : public MetaResource
{
public:
	MetaMesh();
	~MetaMesh();

	GUID Material_ind;

	// More Load Options
};

#endif