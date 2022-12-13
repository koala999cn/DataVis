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
	vertexDecl_->declare(); // 声明vbo数据规格
	prog_->useProgram(); // 激活shader

	glDrawArrays(glModes[type_], 0, vbo_->bytesCount() / vertexDecl_->calcVertexSize());
}
