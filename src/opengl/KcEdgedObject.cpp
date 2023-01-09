#include "KcEdgedObject.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "glad.h"


KcEdgedObject::KcEdgedObject(KePrimitiveType type)
    : super_(type)
{
    edgeShader_ = KsShaderManager::singleton().programFlat(); // 设置缺省的edge渲染程序
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
        if (fill_) {
            glPolygonOffset(-1, -1);
            glEnable(GL_POLYGON_OFFSET_FILL);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(edgeWidth_);
        edgeShader_->useProgram();
        setUniforms_(edgeShader_);
        drawVbo_(); // 使用已绑定的vbo
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 恢复渲染状态

        if (fill_) {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
}
