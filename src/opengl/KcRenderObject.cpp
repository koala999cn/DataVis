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

	// 给shader的uniform赋值
	auto loc = prog_->getUniformLocation("matMvp");
	glUniformMatrix4fv(loc, 1, GL_TRUE, projMat_.data());

	GLint enableClip = !clipBox_.isNull();
	loc = prog_->getUniformLocation("iEnableClip");
	if (loc != -1) {
		glUniform1i(loc, enableClip);
		if (enableClip) {
			loc = prog_->getUniformLocation("vClipLower");
			glUniform3f(loc, clipBox_.lower().x(), clipBox_.lower().y(), clipBox_.lower().z());
			loc = prog_->getUniformLocation("vClipUpper");
			glUniform3f(loc, clipBox_.upper().x(), clipBox_.upper().y(), clipBox_.upper().z());
		}
	}

	loc = prog_->getUniformLocation("vColor");
	if (loc != -1)
	    glUniform4f(loc, color_[0], color_[1], color_[2], color_[3]);

	if (ibo_ && indexCount_ > 0) {
		ibo_->bind();
		auto idxSize = ibo_->bytesCount() / indexCount_;
		GLenum type = (idxSize == 4) ? GL_UNSIGNED_INT : (idxSize == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;
		glDrawElements(glModes[type_], indexCount_, type, 0);
	}
	else {
		glDrawArrays(glModes[type_], 0, vbo_->bytesCount() / vtxDecl_->vertexSize());
	}
}
