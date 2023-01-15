#include "KcLineObject.h"
#include "glad.h"
#include "KuOglUtil.h"


KcLineObject::KcLineObject(KePrimitiveType type)
    : super_(type)
{

}


void KcLineObject::draw() const
{
    glLineWidth(lineWidth_);
    KuOglUtil::glLineStyle(lineStyle_);
    super_::draw();
}

