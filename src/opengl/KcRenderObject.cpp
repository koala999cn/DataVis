#include "KcRenderObject.h"
#include "glad.h"
#include "KcGpuBuffer.h"
#include "KcGlslProgram.h"
#include "KcVertexDeclaration.h"


void KcRenderObject::draw() const
{
	const static GLenum glModes[] = {
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON
	};

	// TODO: 此处没有保存和恢复render状态

	vbo_->bind(); // 激活vbo
	vtxDecl_->declare(); // 声明vbo数据规格
	prog_->useProgram(); // 激活shader
	glUniformMatrix4fv(0, 1, GL_TRUE, projMat_.data());

	GLint enableClip = !clipBox_.isNull();
	auto loc = prog_->getUniformLocation("iEnableClip");
	glUniform1i(loc, enableClip);
	if (enableClip) {
		loc = prog_->getUniformLocation("vClipLower");
		glUniform3f(loc, clipBox_.lower().x(), clipBox_.lower().y(), clipBox_.lower().z());
		loc = prog_->getUniformLocation("vClipUpper");
		glUniform3f(loc, clipBox_.upper().x(), clipBox_.upper().y(), clipBox_.upper().z());
	}


	glDrawArrays(glModes[type_], 0, vbo_->bytesCount() / vtxDecl_->calcVertexSize());
}
