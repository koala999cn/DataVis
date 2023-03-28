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
    // ��5��vbo
    // loc0: �洢��׼�������ݣ�float3��
    // loc1: �洢��׼uv���ݣ�float2��
    // loc2: �洢��ʵ����λ�����ݣ�float3��
    // loc3: �洢��ʵ����uv���ݣ�float4��
    // loc4: �洢��ʵ���ĳߴ����ݣ�float������ѡ 
    // loc5: �洢��ʵ�������ɫ���ݣ�float4������ѡ
    vbos_.resize(5);

    // loc0 & loc1
    vbos_[0].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[0].decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
    vbos_[0].decl->pushAttribute(KcVertexAttribute::k_float2, KcVertexAttribute::k_texcoord);

    // loc2
    vbos_[1].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[1].decl->pushAttribute(KcVertexAttribute(2, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_instance, 1));

    // loc3
    vbos_[2].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[2].decl->pushAttribute(KcVertexAttribute(3, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));

    // loc4
    vbos_[3].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[3].decl->pushAttribute(KcVertexAttribute(4, KcVertexAttribute::k_float, 0, KcVertexAttribute::k_instance, 1));
    vbos_[3].decl->getAttribute(0).enable(false);

    // loc5
    vbos_[4].decl = std::make_shared<KcVertexDeclaration>();
    vbos_[4].decl->pushAttribute(KcVertexAttribute(5, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));
    vbos_[4].decl->getAttribute(0).enable(false);

    for (unsigned i = 0; i < vbos_.size(); i++)
        vbos_[i].buf = std::make_shared<KcGpuBuffer>();

    // ������׼�����uv����
    struct KpVertex_
    {
        point3f pos;
        point2f uv;
    };

    KpVertex_ quad[4];  
    quad[0].pos = { -0.5f,  0.5f, 0.f }; quad[0].uv = { -0.5f,  0.5f }; // ����   
    quad[1].pos = {  0.5f,  0.5f, 0.f }; quad[1].uv = {  0.5f,  0.5f }; // ����  
    quad[2].pos = {  0.5f, -0.5f, 0.f }; quad[2].uv = {  0.5f, -0.5f }; // ���� 
    quad[3].pos = { -0.5f, -0.5f, 0.f }; quad[3].uv = { -0.5f, -0.5f }; // ����

    vbos_[0].buf->setData(quad, sizeof(quad), KcGpuBuffer::k_static_draw);
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


void KcTextObject::setBufferData(const point3f* anchors, const point4f* uvs, unsigned count)
{
    vbos_[1].buf->setData(anchors, count * sizeof(point3f), KcGpuBuffer::k_stream_draw);
    vbos_[2].buf->setData(uvs, count * sizeof(point4f), KcGpuBuffer::k_stream_draw);
    instances_ = count;
}
