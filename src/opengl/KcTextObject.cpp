#include "KcTextObject.h"
#include "glad.h"
#include "opengl/KcGlslProgram.h"
#include <assert.h>


KcTextObject::KcTextObject(int texId, int index)
    : super_(k_quads)
    , texId_(texId)
    , unitIdx_(index)
{

}


void KcTextObject::draw() const
{
    GLint params;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &params);
    assert(params == GL_TRUE);
    assert(glIsTexture((GLuint)texId_));

    glActiveTexture(GL_TEXTURE0 + unitIdx_);
    glBindTexture(GL_TEXTURE_2D, texId_);
    
    auto loc = prog_->getUniformLocation("Texture");
    assert(loc != -1);

    prog_->useProgram();
    glUniform1i(loc, unitIdx_);

    super_::draw();
}


KcRenderObject* KcTextObject::clone() const
{
    auto obj = new KcTextObject(texId_, unitIdx_);
    super_::cloneTo_(*obj);
    return obj;
}
