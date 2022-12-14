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
    prog_->useProgram();
    glUniform4f(1, pointColor_[0], pointColor_[1], pointColor_[2], pointColor_[3]);
    super_::draw();
}
