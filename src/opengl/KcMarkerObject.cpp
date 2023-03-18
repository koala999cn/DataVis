#include "KcMarkerObject.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"


void KcMarkerObject::draw() const
{
    prog_->useProgram();
    auto loc = prog_->getUniformLocation("fScale");
    assert(loc != -1);
    glUniform1f(loc, size_);

    super_::draw();
}


KcRenderObject* KcMarkerObject::clone() const
{
    auto obj = new KcMarkerObject(type_);
    super_::cloneTo_(*obj);
    return obj;
}
