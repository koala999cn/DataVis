#include "KcPointObject.h"
#include "glad.h"


KcPointObject::KcPointObject()
    : super_(k_points)
{

}


void KcPointObject::draw() const
{
    glPointSize(pointSize_);
    super_::draw();
}
