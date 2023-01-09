#include "KcEdgedObject.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "glad.h"


KcEdgedObject::KcEdgedObject(KePrimitiveType type)
    : super_(type)
{
    edgeShader_ = KsShaderManager::singleton().programFlat(); // ����ȱʡ��edge��Ⱦ����
}


void KcEdgedObject::draw() const
{
    if (fill_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        super_::draw();
    }
    else {
        bindVbo_();
    }

    if (edge_ && edgeShader_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(edgeWidth_);
        edgeShader_->useProgram();
        setUniforms_(edgeShader_);
        drawVbo_(); // ʹ���Ѱ󶨵�vbo
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // �ָ���Ⱦ״̬
    }
}
