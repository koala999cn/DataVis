#include "KcMarkerObject.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"


void KcMarkerObject::draw() const
{
    prog_->useProgram();
    auto loc = prog_->getUniformLocation("vScale");
    assert(loc != -1);
    glUniform2f(loc, size_.x(), size_.y());

    super_::draw();
}


KcRenderObject* KcMarkerObject::clone() const
{
    auto obj = new KcMarkerObject(type_);
    super_::cloneTo_(*obj);
    obj->size_ = size_;
    return obj;
}
