#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

using namespace std;

class GameObject;
class Transform;
class Shader;

/**
 *	Axis Aligned Bounding Box
 *	default axis are the world axis (OX, OY, OZ)
 *	using a rotation quaternion a different axis can be provided
 */
class DLLExport AABB
{
	public:
		AABB(GameObject *obj);
		~AABB();

		static void Init();
		void Render(const Shader *shader) const;
		void Update();
		void Update(glm::quat rotationQ);
		void InitAABB();
		bool Overlaps(AABB *aabb);

	public:
		Transform *transform;
		GameObject *obj;

	private:
		glm::vec3 center;
		glm::vec3 halfSize;
};