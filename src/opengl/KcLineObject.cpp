#include "KcLineObject.h"
#include "glad.h"


KcLineObject::KcLineObject(KePrimitiveType type)
    : super_(type)
{

}


void KcLineObject::draw() const
{
    glLineWidth(lineWidth_);
    // TODO: glLineStyle
    super_::draw();
}

