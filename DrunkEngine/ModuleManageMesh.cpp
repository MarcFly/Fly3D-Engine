#include "ModuleManageMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "SDL/include/SDL_opengl.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "ModuleRenderer3D.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ModuleManageMesh::ModuleManageMesh(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

}

bool ModuleManageMesh::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleManageMesh::Start()
{
	bool ret = true;

	//LoadFBX("./glass cube.fbx");
	//LoadFBX("./Dragon 2.5_fbx.fbx");
	//LoadFBX("./Toilet.fbx");
	LoadFBX("./BakerHouse.fbx");
	//LoadFBX("./warrior.fbx");

	return ret;
}

bool ModuleManageMesh::CleanUp()
{
	bool ret = false;

	// detach log streamW
	aiDetachAllLogStreams();

	return ret;
}

bool ModuleManageMesh::LoadFBX(const char* file_path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	
	obj_data add_obj;

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			if(scene->mMeshes[i]->mColors[0] != nullptr)
			{
				add.mesh_color[0] = scene->mMeshes[i]->mColors[0]->r;
				add.mesh_color[1] = scene->mMeshes[i]->mColors[0]->g;
				add.mesh_color[2] = scene->mMeshes[i]->mColors[0]->b;
				add.mesh_color[3] = scene->mMeshes[i]->mColors[0]->a;
			}
			else
			{
				add.mesh_color[0] = 1;
				add.mesh_color[1] = 1;
				add.mesh_color[2] = 1;
				add.mesh_color[3] = 1;
			}
      
			add.num_vertex = scene->mMeshes[i]->mNumVertices;
			add.vertex = new float[add.num_vertex*3];

			memcpy(add.vertex, scene->mMeshes[i]->mVertices, 3*sizeof(float)*add.num_vertex);

			PLOG("New mesh with %d vertices", add.num_vertex)

			if (scene->mMeshes[i]->HasFaces())
			{
				add.num_index = scene->mMeshes[i]->mNumFaces*3;
				add.index = new GLuint[add.num_index];

				add.num_normal = add.num_index * 2;
				add.normal = new float[add.num_normal];
        
				float v1 = 0;
				float v2 = 0;
				float v3 = 0;
				float aux[9];
				int auxloop = 0;

				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						PLOG("WARNING, geometry face with != 3 indices!");
						ret = false; //if we want to stop load
					}
					else
					{
						memcpy(&add.index[j*3], scene->mMeshes[i]->mFaces[j].mIndices, 3*sizeof(GLuint));
					
						float aux[9];

						aux[0] = add.vertex[add.index[j * 3] * 3];
						aux[1] = add.vertex[(add.index[j * 3] * 3) + 1];
						aux[2] = add.vertex[(add.index[j * 3] * 3) + 2];
						aux[3] = add.vertex[(add.index[(j * 3) + 1] * 3)];
						aux[4] = add.vertex[(add.index[(j * 3) + 1] * 3) + 1];
						aux[5] = add.vertex[(add.index[(j * 3) + 1] * 3) + 2];
						aux[6] = add.vertex[(add.index[(j * 3) + 2] * 3)];
						aux[7] = add.vertex[(add.index[(j * 3) + 2] * 3) + 1];
						aux[8] = add.vertex[(add.index[(j * 3) + 2] * 3) + 2];

						float p1 = (aux[0] + aux[3] + aux[6]) / 3;
						float p2 = (aux[1] + aux[4] + aux[7]) / 3;
						float p3 = (aux[2] + aux[5] + aux[8]) / 3;

						add.normal[j * 6] = p1;
						add.normal[j * 6 + 1] = p2;
						add.normal[j * 6 + 2] = p3;

						vec v1(aux[0], aux[1], aux[2]);
						vec v2(aux[3], aux[4], aux[5]);
						vec v3(aux[6], aux[7], aux[8]);

						vec norm = (v2 - v1).Cross(v3 - v1);
						norm.Normalize();

						add.normal[j * 6 + 3] = p1 + norm.x;
						add.normal[j * 6 + 4] = p2 + norm.y;
						add.normal[j * 6 + 5] = p3 + norm.z;
							
					}
				}
			}
			PLOG("Said mesh starts with %d indices", add.num_index)

			SetTexCoords(&add, scene->mMeshes[i]);

			GenBuffers(add);

			//add.parent = &add_obj;

			add_obj.meshes.push_back(add);
		}

		SetupTex(add_obj);

		Objects.push_back(add_obj);
		
		// Set Parenting
		std::vector<obj_data>::iterator item = --Objects.end();
		for (int k = 0; k < item->meshes.size(); k++)
		{
			item->meshes[k].parent = item._Ptr;
		}
		aiReleaseImport(scene);
	}
	else
	{
		PLOG("Error loading scene %s", file_path);
		ret = false;
	}

	return ret;
}

bool ModuleManageMesh::SetTexCoords(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;
	
	// Set TexCoordinates
	if (cpy_data->HasTextureCoords(0))
	{
		mesh->tex_coords = new float[mesh->num_vertex * 2];
		for (int i = 0; i < mesh->num_vertex; i++)
		{
			mesh->tex_coords[i*2] = cpy_data->mTextureCoords[0][i].x;
			mesh->tex_coords[(i*2) + 1] = cpy_data->mTextureCoords[0][i].y;
		}
	}
	else
		PLOG("No texture coordinates to be set");


	return ret;
}

void ModuleManageMesh::DrawMesh(const mesh_data* mesh, bool use_texture) 
{
	// Draw elements
	{

		glColor4f(1, 1, 1, 1);

		
		
		// Bind buffers
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);

		if (use_texture)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, mesh->parent->id_tex);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);	// Set pointers to arrays
		}
		else
			glColor3f(0, 0, 0);

		
		// Draw
		glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

		// Unbind Buffers
		glBindTexture(GL_TEXTURE_2D, 0);
		//glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glColor4f(0, 1, 0, 1);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

	}
}

void ModuleManageMesh::GenBuffers(mesh_data& mesh)
{

	// Vertex Buffer
	glGenBuffers(1, &mesh.id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertex * 3, mesh.vertex, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	glGenBuffers(1, &mesh.id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_index, mesh.index, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}

bool ModuleManageMesh::SetColors(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;

	return ret;
}

void ModuleManageMesh::SetupTex(obj_data& obj)
{
	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &obj.id_tex);
	glBindTexture(GL_TEXTURE_2D, obj.id_tex);

	// Texture Wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_HEIGHT, CHECKERS_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->renderer3D->checkTexture);

	// **Unbind Buffer**
	glBindTexture(GL_TEXTURE_2D, 0);
}