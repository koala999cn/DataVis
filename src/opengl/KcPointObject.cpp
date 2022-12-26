#include "KcPointObject.h"
#include "glad.h"
#include "KsShaderManager.h"


KcPointObject::KcPointObject()
    : super_(k_points)
{
    prog_ = KsShaderManager::singleton().programMono();
}


void KcPointObject::draw() const
{
    glPointSize(pointSize_);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    super_::draw();
}
