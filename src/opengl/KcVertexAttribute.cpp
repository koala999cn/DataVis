#include "KcVertexAttribute.h"
#include <assert.h>


KcVertexAttribute::KcVertexAttribute(unsigned loc, KeFormat fmt, unsigned offset, 
	KeSemantic semantic, unsigned semanticIndex)
{ 
	location_ = loc;
	format_ = fmt;
	bytesOffset_ = offset;
	semantic_ = semantic;
	semanticIndex_ = semanticIndex;
}


unsigned KcVertexAttribute::byteSize() const
{
	switch(format_)
    {
	case k_short1:
	case k_short1_norm:	
		return 2;

	case k_float:
	case k_ubyte4:
	case k_ubyte4_norm:
	case k_short2:
	case k_short2_norm:
		return 4;

	case k_short3:
	case k_short3_norm:
		return 6;

	case k_float2:
	case k_short4:
	case k_short4_norm:
		return 8;

	case k_float3:	
		return 12;

	case k_float4:
		return 16;
    }
    
	assert(false);
    return 0;
}


KcVertexAttribute::KeFormat KcVertexAttribute::baseType() const
{
	switch(format_)
    {
	case k_float1:
	case k_float2:
	case k_float3:
	case k_float4:
		return k_float;

	case k_ubyte4:
	case k_ubyte4_norm:
		return k_ubyte4;

	case k_short1:
	case k_short2:
	case k_short2_norm:
	case k_short3:
	case k_short4:
	case k_short4_norm:
		return k_short;

	default:
		break;
    }
    
    return format_;
}


unsigned KcVertexAttribute::componentCount() const
{
    switch(format_)
    {
	case k_short1:
	case k_short1_norm:
    case k_float1:		
		return 1;

    case k_float2:
    case k_short2:
    case k_short2_norm:  
		return 2;

	case k_short3:
	case k_short3_norm:
    case k_float3:		
		return 3;

    case k_float4:
    case k_ubyte4:
	case k_ubyte4_norm:
    case k_short4:
    case k_short4_norm:  
		return 4;
    }
 
	assert(false);
    return 0;
}


unsigned KcVertexAttribute::componentByteSize() const
{
    switch(format_)
    {
    case k_float1: 
    case k_float2:
    case k_float3:
    case k_float4:
		return 4;

    case k_ubyte4:    
    case k_ubyte4_norm:  
		return 1;
	
	case k_short1:
	case k_short2:
	case k_short3:
    case k_short4:
	case k_short1_norm:
	case k_short2_norm:
	case k_short3_norm:
	case k_short4_norm:
		return 2;
    }
    
    return 0;
}


bool KcVertexAttribute::normalized() const
{
	switch (format_)
	{
	case k_ubyte4_norm:
	case k_short1_norm:
	case k_short2_norm:
	case k_short3_norm:
	case k_short4_norm:
		return true;

	default:
		break;
	}

	return false;
}
