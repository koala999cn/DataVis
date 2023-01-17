#include "KcEdgedObject.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "KcVertexDeclaration.h"
#include "glad.h"
#include "KuOglUtil.h"


KcEdgedObject::KcEdgedObject(KePrimitiveType type)
    : super_(type)
{
    edgeShader_ = KsShaderManager::singleton().progMono(); // ����ȱʡ��edge��Ⱦ����
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

            // ����������û��color������ɫ����fill������������edge��ɫ
            if (!vtxDecl_->hasColor()) {
                auto loc = edgeShader_->getUniformLocation("vColor");
                if (loc != -1)
                    glUniform4f(loc, edgeColor_[0], edgeColor_[1], edgeColor_[2], edgeColor_[3]);
            }

            drawVbo_(); // ʹ���Ѱ󶨵�vbo
        }
        else {
            super_::draw(); // ��ֻ��ߵ�����£�ʹ��fill����
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // �ָ���Ⱦ״̬
    }
}
