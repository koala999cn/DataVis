#include "KcEdgedObject.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "KcVertexDeclaration.h"
#include "glad.h"
#include "KuOglUtil.h"


KcEdgedObject::KcEdgedObject(KePrimitiveType type)
    : super_(type)
{
    // NB: �˴��ٶ�clipBoxʼ��Ϊfalse��������Ϊtrue����ᵼ��plot2d��edge����������ʾ
    edgeShader_ = KsShaderManager::singleton().progMono(false); // ����ȱʡ��edge��Ⱦ����
}


void KcEdgedObject::draw() const
{
    bool hasEdge = edge_ && edgeShader_;

    if (fill_) {
        if (hasEdge) {
            glPolygonOffset(1, 1);
            glEnable(GL_POLYGON_OFFSET_FILL);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        super_::draw();

        if (hasEdge) {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    else if (hasEdge) {
        bindVbo_();
    }

    if (hasEdge) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(edgeWidth_);
        KuOglUtil::glLineStyle(edgeStyle_);

        if (fill_) {
            edgeShader_->useProgram();
            setUniforms_(edgeShader_);

            // ��fill������£�ʼ���ø�ɫ���
            // �˴�����setUniforms_���õ���ɫ
            auto loc = edgeShader_->getUniformLocation("vColor");
            if (loc != -1)
                glUniform4f(loc, edgeColor_[0], edgeColor_[1], edgeColor_[2], edgeColor_[3]); 
  
            drawVbo_(); // ʹ���Ѱ󶨵�vbo
        }
        else {
            super_::draw(); // ��ֻ��ߵ�����£�ʹ��fill����
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // �ָ���Ⱦ״̬
    }
}



KcRenderObject* KcEdgedObject::clone() const
{
    auto obj = new KcEdgedObject(type_);
    obj->edgeShader_ = edgeShader_;
    obj->edgeWidth_ = edgeWidth_;
    obj->edgeStyle_ = edgeStyle_;
    obj->edgeColor_ = edgeColor_;
    obj->fill_ = fill_;
    obj->edge_ = edge_;
    cloneTo_(*obj);
    return obj;
}
