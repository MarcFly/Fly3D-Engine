#ifndef _COMPONENT_SKELETON_
#define _COMPONENT_SKELETON_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "Component.h"

class GameObject;
struct ResourceSkeleton;

class ComponentSkeleton : public Component
{
public:
	ComponentSkeleton();
	ComponentSkeleton(GameObject* par);

	~ComponentSkeleton() {};

	void Draw();

	void CleanUp();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

	bool CheckSkeletonValidity();
	void LinkMesh();

public:
	ResourceSkeleton* r_skel;
	ComponentMesh* c_mesh;

public:
	void SetBaseVals()
	{
		type = CT_Skeleton;
		multiple = false;
		to_pop = false;
		c_mesh = nullptr;
		r_skel = nullptr;
	}
};

#endif