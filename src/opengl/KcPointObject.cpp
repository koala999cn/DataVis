#include "KcPointObject.h"
#include "glad.h"
#include "KsShaderManager.h"
#include "opengl/KcGlslProgram.h"


KcPointObject::KcPointObject()
    : super_(k_points, KsShaderManager::singleton().programMono())
{

}


void KcPointObject::draw() const
{
    glPointSize(pointSize_);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    prog_->useProgram();
    auto loc = prog_->getUniformLocation("vColor");
    glUniform4f(loc, pointColor_[0], pointColor_[1], pointColor_[2], pointColor_[3]);
    super_::draw();
}
