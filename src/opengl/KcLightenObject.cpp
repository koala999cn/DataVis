#include "KcLightenObject.h"
#include "KsShaderManager.h"
#include "KcGlslProgram.h"
#include "glad.h"
#include <assert.h>


KcLightenObject::KcLightenObject(KePrimitiveType type)
    : super_(type)
{
    prog_ = KsShaderManager::singleton().programMonoLight();
}


void KcLightenObject::draw() const
{
    auto loc = prog_->getUniformLocation("matNormal");
    assert(loc != -1);
    
    prog_->useProgram();
    glUniformMatrix4fv(loc, 1, GL_TRUE, normalMat_.data());
    //glProgramUniformMatrix4fv(prog_->handle(), loc, 1, GL_TRUE, normalMat_.data());

    super_::draw();
}
