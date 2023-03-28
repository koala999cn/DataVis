#include "KcTextObject.h"
#include "glad.h"
#include "KcGlslProgram.h"
#include "KcVertexDeclaration.h"
#include "KcGpuBuffer.h"
#include <assert.h>


KcTextObject::KcTextObject(int texId, int index)
    : super_(k_quads)
    , texId_(texId)
    , unitIdx_(index)
{
    // ��6��vbo
    // loc0: �洢��׼�������ݣ�float4��
    // loc1: �洢��ʵ����anchor���ݣ�float3��
    // loc2: �洢��ʵ����ƫ�����ݣ�float4����dx0, dy0, dx1, dy1����������
    // loc3: �洢��ʵ����uv���ݣ�float4����u0, v0, u1, v1����һ������
    // loc4: �洢��ʵ���ĳߴ����ݣ�float������ѡ 
    // loc5: �洢��ʵ�������ɫ���ݣ�float4������ѡ
    vbos_.resize(6);

    // loc0
    vbos_[0].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[0].decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_texcoord); // ʹ��k_texcoord���壬���ڴ���uv���͵�shader

    // loc1
    vbos_[1].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[1].decl->pushAttribute(KcVertexAttribute(1, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_instance, 1));

    // loc2
    vbos_[2].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[2].decl->pushAttribute(KcVertexAttribute(2, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));

    // loc3
    vbos_[3].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[3].decl->pushAttribute(KcVertexAttribute(3, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));

    // loc4
    vbos_[4].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[4].decl->pushAttribute(KcVertexAttribute(4, KcVertexAttribute::k_float, 0, KcVertexAttribute::k_instance, 1));
    vbos_[4].decl->getAttribute(0).enable(false);

    // loc5
    vbos_[5].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[5].decl->pushAttribute(KcVertexAttribute(5, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));
    vbos_[5].decl->getAttribute(0).enable(false);

    for (unsigned i = 0; i < vbos_.size(); i++)
        vbos_[i].buf = std::make_shared<KcGpuBuffer>();

    // ������׼��������

    point4f quad[4];  
    quad[0] = { 1, 1, 0, 0 }; // ����   
    quad[1] = { 0, 1, 1, 0 }; // ����  
    quad[2] = { 0, 0, 1, 1 }; // ���� 
    quad[3] = { 1, 0, 0, 1 }; // ����

    vbos_[0].buf->setData(quad, sizeof(point4f) * 4, KcGpuBuffer::k_static_draw);
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


    int bSizeVarying = vbos_[3].decl->getAttribute(0).enabled();
    loc = prog_->getUniformLocation("bSizeVarying");
    glUniform1i(loc, bSizeVarying);

    int bColorVarying = vbos_[4].decl->getAttribute(0).enabled();
    loc = prog_->getUniformLocation("bColorVarying");
    glUniform1i(loc, bColorVarying);

    loc = prog_->getUniformLocation("vScale");
    //if (bSizeVarying) {
        glUniform3f(loc, scale_.x(), scale_.y(), scale_.z());
    //}
   //else {
    //    glUniform3f(loc, marker_.size * scale_.x(),
    //        marker_.size * scale_.y(),
   //         marker_.size * scale_.z());
   // }

    super_::draw();
}


KcRenderObject* KcTextObject::clone() const
{
    auto obj = new KcTextObject(texId_, unitIdx_);
    super_::cloneTo_(*obj);

    obj->scale_ = scale_;

    return obj;
}


void KcTextObject::setBufferData(const point3f* anchors, const point4f* offset, const point4f* uvs, unsigned count)
{
    vbos_[1].buf->setData(anchors, count * sizeof(point3f), KcGpuBuffer::k_stream_draw);
    vbos_[2].buf->setData(offset, count * sizeof(point4f), KcGpuBuffer::k_stream_draw);
    vbos_[3].buf->setData(uvs, count * sizeof(point4f), KcGpuBuffer::k_stream_draw);
    instances_ = count;
}
