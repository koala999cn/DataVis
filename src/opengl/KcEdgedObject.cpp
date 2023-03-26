#include "KcEdgedObject.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "KcVertexDeclaration.h"
#include "glad.h"
#include "KuOglUtil.h"


KcEdgedObject::KcEdgedObject(KePrimitiveType type)
    : super_(type)
{
    // NB: 此处假定clipBox始终为false，若设置为true，则会导致plot2d的edge不能正常显示
    edgeShader_ = KsShaderManager::singleton().progMono(false); // 设置缺省的edge渲染程序
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

            // 在fill的情况下，始终用辅色描边
            // 此处修正setUniforms_设置的主色
            auto loc = edgeShader_->getUniformLocation("vColor");
            if (loc != -1)
                glUniform4f(loc, edgeColor_[0], edgeColor_[1], edgeColor_[2], edgeColor_[3]); 
  
            drawVbo_(); // 使用已绑定的vbo
        }
        else {
            super_::draw(); // 在只描边的情况下，使用fill配置
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 恢复渲染状态
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
