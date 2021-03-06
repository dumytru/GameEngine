//#include <pch.h>
#include "Material.h"

using namespace std;

void Material::createUBO() {
	glGenBuffers(1, &material_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialBlock), static_cast<MaterialBlock*>(this), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	if (shader->loc_material != INVALID_LOC)
		glUniformBlockBinding(shader->program, shader->loc_material, 0);
};

void Material::bindUBO(GLuint location) {
	if (shader->loc_material != INVALID_LOC)
		glUniformBlockBinding(shader->program, shader->loc_material, location);
}

void Material::linkShader(Shader* const S) {
	shader = S;
}
