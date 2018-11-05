#include "ComponentCamera.h"
#include "GameObject.h"
#include "Application.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.01f
#define MOUSE_WHEEL_SPEED 6.0f

ComponentCamera::ComponentCamera(GameObject * par)
{
	SetBaseVals();

	if (par != nullptr)
	{
		this->parent = par;
		App->scene->active_cameras.push_back(this);
	}
	else
		App->scene->SetmainCam(this);

	CalculateViewMatrix();

	id = App->scene->active_cameras.size();

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Position = vec(0.0f, 0.0f, 5.0f);
	Reference = vec(0.0f, 0.0f, 0.0f);

	frustum.nearPlaneDistance = 0.5f;
	frustum.farPlaneDistance = 500.0f;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.projectiveSpace = FrustumProjectiveSpace::FrustumSpaceGL;
	frustum.handedness = FrustumHandedness::FrustumRightHanded;

	frustum.SetWorldMatrix(float3x4::identity);

	frustum.verticalFov = DegToRad(60.0f);
	SetAspectRatio();

	frustum.SetPerspective(frustum.horizontalFov, frustum.verticalFov);

	frustum.pos = float3::zero;

	frustum.Translate(Position);

	frustum.GetCornerPoints(bb_frustum);

	original_v_fov = frustum.verticalFov;
	original_h_fov = frustum.horizontalFov;

	mesh_multiplier = 1;

}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Start()
{
	// Start
}

void ComponentCamera::Update(const float dt)
{
	float speed = MOV_SPEED * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 2 * MOV_SPEED * dt;

	if (parent->GetTransform()->update_camera_transform)
	{
		TransformPos(parent->GetTransform()->position);
		TransformRot(parent->GetTransform()->rotate_quat);
		TransformScale(parent->GetTransform()->scale);

		frustum.GetCornerPoints(bb_frustum);

		parent->GetTransform()->update_camera_transform = false;
	}

	CalculateViewMatrix();
}

void ComponentCamera::Draw()
{
	if (parent != nullptr)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3f(1.f, 1.f, 1.f);

		glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
		glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);

		glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
		glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

		glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
		glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

		glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);
		glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

		glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
		glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

		glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);
		glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

		glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);
		glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

		glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
		glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

		glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
		glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

		glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
		glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

		glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);
		glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

		glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
		glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

		glEnd();

		if (App->renderer3D->lighting)
			glEnable(GL_LIGHTING);
	}
}

void ComponentCamera::CleanUp()
{
	// Cleanup
}

void ComponentCamera::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	float3 aux = Position - Reference;
	Z = aux.Normalized();
	aux = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = aux.Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}


	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::LookAt(const vec &Spot)
{
	Reference = Spot;

	float3 aux = Position - Reference;
	Z = aux.Normalized();
	aux = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = aux.Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ComponentCamera::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::Transport(const vec &Movement)
{
	Position = Movement;

	CalculateViewMatrix();
}

void ComponentCamera::Rotate()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	if (dx != 0)
	{
		float DeltaX = (float)dx * MOUSE_SENSIBILITY;

		X = RotateAngle(X, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Y = RotateAngle(Y, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Z = RotateAngle(Z, DeltaX, float3(0.0f, 1.0f, 0.0f));

	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * MOUSE_SENSIBILITY;

		Y = RotateAngle(Y, DeltaY, X);
		Z = RotateAngle(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = Z.Cross(X);
		}

	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		Position -= Reference;
		Position = Reference + Z * Position.Length();
	}
}

float3 ComponentCamera::RotateAngle(const float3 &u, float angle, const float3 &v)
{
	return *(float3*)&(float4x4::RotateAxisAngle(v, angle) * float4(u, 1.0f));
}

void ComponentCamera::SetAspectRatio()
{
	float aspect_ratio = ((float)App->window->window_w / (float)App->window->window_h);		//Window aspect ratio
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	frustum.SetPerspective(frustum.horizontalFov, frustum.verticalFov);
}

float * ComponentCamera::GetViewMatrix()
{
	return &ViewMatrix[0][0];
}

void ComponentCamera::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);

	ViewMatrixInverse = ViewMatrix.Inverted();
}

//void ComponentCamera::MoveTest(float speed)
//{
//	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT)
//	{
//		frustum.verticalFov += 0.0015;
//		frustum.GetCornerPoints(bb_frustum);
//		SetAspectRatio();
//	}
//	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT)
//	{
//		frustum.verticalFov -= 0.0015;
//		frustum.GetCornerPoints(bb_frustum);
//		SetAspectRatio();
//	}
//
//	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
//	{
//		frustum.pos -= Z * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
//	{
//		frustum.pos += Z * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
//	{
//		frustum.pos += X * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
//	{
//		frustum.pos -= X * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
//	{
//		frustum.pos -= Y * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
//	{
//		frustum.pos += Y * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
//	{
//		frustum.pos += Y * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
//	{
//		frustum.pos -= Y * speed;
//		frustum.GetCornerPoints(bb_frustum);
//	}
//}

void ComponentCamera::LookToObj(GameObject* obj, float vertex_aux)
{
	Transport(vec(vertex_aux + 3, vertex_aux + 3, vertex_aux + 3));

	vec aux = obj->getObjectCenter();
	LookAt(vec(aux.x, aux.y, aux.z));

	mesh_multiplier = vertex_aux / 4;
}

void ComponentCamera::TransformPos(float3 pos)
{
	frustum.pos = pos;
}

void ComponentCamera::TransformRot(Quat rot)
{

	float3 new_rot = rot.ToEulerXYZ();

	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	Quat rotation_y = Quat::RotateY(new_rot.y);
	frustum.SetFront(rotation_y.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_y.Mul(frustum.Up()).Normalized());

	Quat rotation_x = Quat::RotateAxisAngle(frustum.WorldRight(), new_rot.x);
	frustum.SetFront(rotation_x.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_x.Mul(frustum.Up()).Normalized());

	Quat rotation_z = Quat::RotateZ(new_rot.z);
	frustum.SetFront(rotation_z.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_z.Mul(frustum.Up()).Normalized());
}

void ComponentCamera::TransformScale(float3 scale)
{
	//frustum.verticalFov = original_v_fov * scale.y;
	//frustum.horizontalFov = original_h_fov * scale.x;
}

void ComponentCamera::Load(JSON_Object* comp)
{

}

void ComponentCamera::Save(JSON_Array* comps)
{

}
