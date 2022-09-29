#include "KmFrustum.h"
#include "KtuMath.h"
#include "KiRenderable.h" // TODO
#include "KgRenderQueue.h"
#include "KgVertexBufferBinding.h"
#include "KuDebug.h"

const float KmFrustum::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

KmFrustum::KmFrustum(void) : 
	m_eProjectionType(KPT_PERSPECTIVE), 
	m_fFOVy(KMathf::PI/4.0f), 
	m_fFarDist(100000.0f), 
	m_fNearDist(100.0f), 
	m_fAspectRatio(1.33333333333333f), 
	m_fOrthoHeight(1000.f),
	m_fFocalLength(1.f),
	m_kLastParentOrientation(KQuaternionf::IDENTITY),
	m_f3LastParentPosition(float3::ZERO),
	m_bRecalcFrustum(true), 
	m_bRecalcView(true), 
	m_bRecalcFrustumPlanes(true),
	m_bRecalcWorldSpaceCorners(true),
	m_bRecalcVertexData(true),
	m_bCustomViewMatrix(false),
	m_bCustomProjMatrix(false),
	m_bFrustumExtentsManuallySet(false),
	m_eOrientationMode(KOR_DEGREE_0),
	m_bReflect(false), 
	m_pkLinkedReflectPlane(0),
	m_bObliqueDepthProjection(false),
	m_pkLinkedObliqueProjPlane(0)
{
	m_f2FrustumOffset[0] = m_f2FrustumOffset[1] = 0.f;

	// Initialise material
//	mMaterial = MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");

	// Alter superclass members
//	m_bVisible = false;
//	m_pkParentNode = 0;
//	mName = name;

	UpdateView();
	UpdateFrustum();
}


KmFrustum::~KmFrustum(void)
{
	// Do nothing
}

void KmFrustum::SetFovAngleY(float fFovY)
{
	m_fFOVy = fFovY;

	InvalidateFrustum();
}

float KmFrustum::GetFovAngleY(void) const
{
	return m_fFOVy;
}

void KmFrustum::SetNearClipDistance(float fNearDist)
{
	K_ASSERT(fNearDist >= 0);
	m_fNearDist = fNearDist;

	InvalidateFrustum();
}

float KmFrustum::getNearClipDistance(void) const
{
	return m_fNearDist;
}

void KmFrustum::SetFarClipDistance(float fFarDist)
{
	m_fFarDist = fFarDist;

	InvalidateFrustum();
}

float KmFrustum::GetFarClipDistance(void) const
{
	return m_fFarDist;
}

void KmFrustum::SetAspectRatio(float fRatio)
{
	m_fAspectRatio = fRatio;

	InvalidateFrustum();
}

float KmFrustum::GetAspectRatio(void) const
{
	return m_fAspectRatio;
}

void KmFrustum::SetFrustumOffset(const float offset[2])
{
	SetFrustumOffset(offset[0], offset[1]);
}

void KmFrustum::SetFrustumOffset(float horizontal, float vertical)
{
	m_f2FrustumOffset[0] = horizontal;
	m_f2FrustumOffset[1] = vertical;

	InvalidateFrustum();
}

const float* KmFrustum::GetFrustumOffset() const
{
	return m_f2FrustumOffset;
}


void KmFrustum::SetFocalLength(float fFocalLength)
{
	K_ASSERT(fFocalLength > 0.f);
	m_fFocalLength = fFocalLength;

	InvalidateFrustum();
}

float KmFrustum::GetFocalLength() const
{
	return m_fFocalLength;
}

void KmFrustum::SetFrustumExtents(float left, float right, float top, float bottom)
{
	m_bFrustumExtentsManuallySet = true;
	m_fLeft = left;
	m_fRight = right;
	m_fTop = top;
	m_fBottom = bottom;

	InvalidateFrustum();
}

void KmFrustum::ResetFrustumExtents()
{
	m_bFrustumExtentsManuallySet = false;
	InvalidateFrustum();
}

void KmFrustum::GetFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const
{
	UpdateFrustum();
	outleft = m_fLeft;
	outright = m_fRight;
	outtop = m_fTop;
	outbottom = m_fBottom;
}

const float4x4& KmFrustum::GetProjectionMatrixRS(void) const
{
	UpdateFrustum();

	return m_f4x4ProjMatrixRS;
}

const float4x4& KmFrustum::GetProjectionMatrixWithRSDepth(void) const
{
	UpdateFrustum();

	return m_f4x4ProjMatrixRSDepth;
}

const float4x4& KmFrustum::GetProjectionMatrix(void) const
{
	UpdateFrustum();

	return m_f4x4ProjMatrix;
}

const float4x4& KmFrustum::GetViewMatrix(void) const
{
	UpdateView();

	return m_f4x4ViewMatrix;
}

void KmFrustum::CalcViewMatrixRelative(const float3& f3RelPos, float4x4& f4x4MatToUpdate) const
{
	float4x4 matTrans = float4x4::IDENTITY;
	matTrans.SetTranslation(f3RelPos);
	f4x4MatToUpdate = GetViewMatrix() * matTrans;
}

void KmFrustum::SetCustomViewMatrix(bool bEnable, const float4x4& viewMatrix)
{
	m_bCustomViewMatrix = bEnable;
	if(bEnable)
	{
		K_ASSERT(viewMatrix.IsAffine());
		m_f4x4ViewMatrix = viewMatrix;
	}
	InvalidateView();
}

void KmFrustum::SetCustomProjectionMatrix(bool bEnable, const float4x4& projectionMatrix)
{
	m_bCustomProjMatrix = bEnable;
	if(bEnable)
	{
		m_f4x4ProjMatrix = projectionMatrix;
	}
	InvalidateFrustum();
}

const KPlanef* KmFrustum::GetFrustumPlanes(void) const
{
	// Make any pending updates to the calculated frustum planes
	UpdateFrustumPlanes();

	return m_k6FrustumPlanes;
}

const KPlanef& KmFrustum::GetFrustumPlane( unsigned short plane ) const
{
	K_ASSERT(plane >= 0 && plane < 6);

    // Make any pending updates to the calculated frustum planes
    UpdateFrustumPlanes();

	return m_k6FrustumPlanes[plane];
}

unsigned int KmFrustum::GetTypeFlags(void) const
{
	K_ASSERT(false);
	return 0;
}

const KtAxisAlignedBox<float>& KmFrustum::GetBoundingBox(void) const
{
	return m_kBoundingBox;
}

float KmFrustum::GetBoundingRadius(void) const
{
	return (m_fFarDist == 0)? 100000 : m_fFarDist;
}

void KmFrustum::_UpdateRenderQueue(KgRenderQueue* queue)
{
//	if(m_bDebugDisplay)
	{
		// Add self 
//		queue->AddRenderable(this);
	}
}

void KmFrustum::_NotifyCurrentCamera(KcCamera* pkCamera)
{

}

void KmFrustum::GetRenderOperation(KpRenderOperation& op)
{
	UpdateVertexData();
	op.m_eTopology = KeTopology::KT_LINE_LIST;
	op.m_bUseIndexes = false;
	op.m_bUseGlobalInstancingVertexBufferIsAvailable = false;
	op.m_pkVertexData = &m_kpVertexData;
}

void KmFrustum::GetWorldTransforms(float4x4* xform) const
{
//	if(m_pkParentNode)
//		*xform = m_pkParentNode->_GetFullTransform();
//	else
//		*xform = float4x4::IDENTITY;
}

float KmFrustum::GetSquaredViewDepth(const KcCamera* cam) const
{

}

const float3* KmFrustum::GetWorldSpaceCorners(void) const
{
	UpdateWorldSpaceCorners();

	return m_f3WorldSpaceCorners;
}

void KmFrustum::SetProjectionType(KeProjectionType pt)
{
	m_eProjectionType = pt;
	InvalidateFrustum();
}

KeProjectionType KmFrustum::GetProjectionType(void) const
{
	return m_eProjectionType;
}

void KmFrustum::SetOrthoWindow(float w, float h)
{
	m_fOrthoHeight = h;
	m_fAspectRatio = w / h;
	InvalidateFrustum();
}

void KmFrustum::SetOrthoWindowHeight(float h)
{
	m_fOrthoHeight = h;
	InvalidateFrustum();
}

void KmFrustum::SetOrthoWindowWidth(float w)
{
	m_fOrthoHeight = w / m_fAspectRatio;
	InvalidateFrustum();
}

float KmFrustum::GetOrthoWindowHeight() const
{
	return m_fOrthoHeight;
}

float KmFrustum::GetOrthoWindowWidth() const
{
	return m_fOrthoHeight * m_fAspectRatio;
}

void KmFrustum::EnableReflection(const KPlanef& p)
{
	m_bReflect = true;
	m_kReflectPlane = p;
//	m_pkLinkedReflectPlane = 0;
//	m_f4x4ReflectMatrix = KMathf::BuildReflectionMatrix(p);
	InvalidateView();
}

void KmFrustum::DisableReflection(void)
{

}

void KmFrustum::EnableCustomNearClipPlane(const KPlanef& plane)
{
	m_bObliqueDepthProjection = true;
//	m_pkLinkedObliqueProjPlane = 0;
	m_kObliqueProjPlane = plane;
	InvalidateFrustum();
}

void KmFrustum::DisableCustomNearClipPlane(void)
{

}

const float3& KmFrustum::GetPositionForViewUpdate(void) const
{
	return m_f3LastParentPosition;
}

const KQuaternionf& KmFrustum::GetOrientationForViewUpdate(void) const
{
	return m_kLastParentOrientation;
}

void KmFrustum::SetOrientationMode(KeOrientationMode eMode)
{
#if OGRE_NO_VIEWPORT_ORIENTATIONMODE != 0
	OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
				"Setting Frustrum orientation mode is not supported",
				__FUNCTION__);
#endif
	m_eOrientationMode = eMode;
	InvalidateFrustum();
}

KeOrientationMode KmFrustum::GetOrientationMode() const
{
#if OGRE_NO_VIEWPORT_ORIENTATIONMODE != 0
	OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
				"Getting Frustrum orientation mode is not supported",
				__FUNCTION__);
#endif
	return m_eOrientationMode;
}

void KmFrustum::CalcProjectionParameters(float& left, float& right, float& bottom, float& top) const
{
	if(m_bCustomProjMatrix)
	{
		// Convert clipspace corners to camera space
		float4x4 f4InvProj = m_f4x4ProjMatrix.GetInverse();
		float3 topLeft(-0.5f, 0.5f, 0.0f);
		float3 bottomRight(0.5f, -0.5f, 0.0f);

		topLeft = f4InvProj * topLeft;
		bottomRight = f4InvProj * bottomRight;

		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}
	else
	{
		if(m_bFrustumExtentsManuallySet)
		{
			left = m_fLeft;
			right = m_fRight;
			top = m_fTop;
			bottom = m_fBottom;
		}
		// Calculate general projection parameters
		else if(m_eProjectionType == KPT_PERSPECTIVE)
		{
			float thetaY (m_fFOVy * 0.5f);
			float tanThetaY = KMathf::Tan(thetaY);
			float tanThetaX = tanThetaY * m_fAspectRatio;

			float nearFocal = m_fNearDist / m_fFocalLength;
			float nearOffsetX = m_f2FrustumOffset[0] * nearFocal;
			float nearOffsetY = m_f2FrustumOffset[1] * nearFocal;
			float half_w = tanThetaX * m_fNearDist;
			float half_h = tanThetaY * m_fNearDist;

			left   = - half_w + nearOffsetX;
			right  = + half_w + nearOffsetX;
			bottom = - half_h + nearOffsetY;
			top    = + half_h + nearOffsetY;

			m_fLeft = left;
			m_fRight = right;
			m_fTop = top;
			m_fBottom = bottom;
		}
		else
		{
			// Unknown how to apply frustum offset to orthographic camera, just ignore here
			float half_w = GetOrthoWindowWidth() * 0.5f;
			float half_h = GetOrthoWindowHeight() * 0.5f;

			left   = - half_w;
			right  = + half_w;
			bottom = - half_h;
			top    = + half_h;

			m_fLeft = left;
			m_fRight = right;
			m_fTop = top;
			m_fBottom = bottom;
		}
	}
}

void KmFrustum::UpdateFrustum(void) const
{
	if(IsFrustumOutOfDate())
	{
		UpdateFrustumImpl();
	}
}

void KmFrustum::UpdateView(void) const
{

}

void KmFrustum::UpdateFrustumImpl(void) const
{
	// Common calcs
	float left, right, bottom, top;

#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
    if(m_eOrientationMode != KOR_PORTRAIT)
        CalcProjectionParameters(bottom, top, left, right);
    else
#endif
        CalcProjectionParameters(left, right, bottom, top);

	if(!m_bCustomProjMatrix)
	{

		// The code below will dealing with general projection 
		// parameters, similar glFrustum and glOrtho.
		// Doesn't optimise manually except division operator, so the 
		// code more self-explaining.

		float inv_w = 1 / (right - left);
		float inv_h = 1 / (top - bottom);
		float inv_d = 1 / (m_fFarDist - m_fNearDist);

		// Recalc if frustum params changed
		if(m_eProjectionType == KPT_PERSPECTIVE)
		{
			// Calc matrix elements
			float A = 2 * m_fNearDist * inv_w;
			float B = 2 * m_fNearDist * inv_h;
			float C = (right + left) * inv_w;
			float D = (top + bottom) * inv_h;
			float q, qn;
			if(m_fFarDist == 0)
			{
				// Infinite far plane
				q = INFINITE_FAR_PLANE_ADJUST - 1;
				qn = m_fNearDist * (INFINITE_FAR_PLANE_ADJUST - 2);
			}
			else
			{
				q = - (m_fFarDist + m_fNearDist) * inv_d;
				qn = -2 * (m_fFarDist * m_fNearDist) * inv_d;
			}

			// NB: This creates 'uniform' perspective projection matrix,
			// which depth range [-1,1]
			//
			// right-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   q   qn ]
			// [ 0   0   -1  0  ]
			//
			// left-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   -q  qn ]
			// [ 0   0   1   0  ]
			//
			// A = 2 * near / (right - left)
			// B = 2 * near / (top - bottom)
			// C = (right + left) / (right - left)
			// D = (top + bottom) / (top - bottom)
			// q = - (far + near) / (far - near)
			// qn = - 2 * (far * near) / (far - near)

			float fSign = KtVector3<float>::FORWARD.z; // 右手系的时候为-1，左手系的时候为+1

			m_f4x4ProjMatrix = KtMatrix4<float>::ZERO;
			m_f4x4ProjMatrix.m00() = A;
			m_f4x4ProjMatrix.m02() = C;
			m_f4x4ProjMatrix.m11() = B;
			m_f4x4ProjMatrix.m12() = D;
			m_f4x4ProjMatrix.m22() = q * -fSign;
			m_f4x4ProjMatrix.m23() = qn;
			m_f4x4ProjMatrix.m32() = fSign;

			if(m_bObliqueDepthProjection)
			{
				// Translate the plane into view space

				// Don't use getViewMatrix here, incase overrided by 
				// camera and return a cull frustum view matrix
				UpdateView();
				KPlanef plane = m_f4x4ViewMatrix * m_kObliqueProjPlane;

				// Thanks to Eric Lenyel for posting this calculation 
				// at http://www.terathon.com

				// Calculate the clip-space corner point opposite the 
				// clipping plane
				// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
				// transform it into camera space by multiplying it
				// by the inverse of the projection matrix

				/* generalised version
				Vector4 q = matrix.inverse() * 
				Vector4(Math::Sign(plane.normal.x), 
				Math::Sign(plane.normal.y), 1.0f, 1.0f);
				*/
				float4 qVec;
				qVec.x = (KMathf::Sign(plane.Normal().x) + m_f4x4ProjMatrix.m02()) / m_f4x4ProjMatrix.m00();
				qVec.y = (KMathf::Sign(plane.Normal().y) + m_f4x4ProjMatrix.m12()) / m_f4x4ProjMatrix.m11();
				qVec.z = -1;
				qVec.w = (1 + m_f4x4ProjMatrix.m22()) / m_f4x4ProjMatrix.m23();

				// Calculate the scaled plane vector
				float4 clipPlane4d(plane.Normal().x, plane.Normal().y, plane.Normal().z, plane.Distance());
				float4 c = clipPlane4d * (2 / (clipPlane4d.Dot(qVec)));

				// Replace the third row of the projection matrix
				m_f4x4ProjMatrix.m20() = c.x;
				m_f4x4ProjMatrix.m21() = c.y;
				m_f4x4ProjMatrix.m22() = c.z + 1;
				m_f4x4ProjMatrix.m23() = c.w; 
			}
		} // perspective
		else if(m_eProjectionType == KPT_ORTHOGRAPHIC)
		{
			float A = 2 * inv_w;
			float B = 2 * inv_h;
			float C = - (right + left) * inv_w;
			float D = - (top + bottom) * inv_h;
			float q, qn;
			if(m_fFarDist == 0)
			{
				// Can not do infinite far plane here, avoid divided zero only
				q = - INFINITE_FAR_PLANE_ADJUST / m_fNearDist;
				qn = - INFINITE_FAR_PLANE_ADJUST - 1;
			}
			else
			{
				q = - 2 * inv_d;
				qn = - (m_fFarDist + m_fNearDist)  * inv_d;
			}

			// NB: This creates 'uniform' orthographic projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   0   C  ]
			// [ 0   B   0   D  ]
			// [ 0   0   q   qn ]
			// [ 0   0   0   1  ]
			//
			// A = 2 * / (right - left)
			// B = 2 * / (top - bottom)
			// C = - (right + left) / (right - left)
			// D = - (top + bottom) / (top - bottom)
			// q = - 2 / (far - near)
			// qn = - (far + near) / (far - near)

			m_f4x4ProjMatrix = KtMatrix4<float>::ZERO;
			m_f4x4ProjMatrix.m00() = A;
			m_f4x4ProjMatrix.m03() = C;
			m_f4x4ProjMatrix.m11() = B;
			m_f4x4ProjMatrix.m13() = D;
			m_f4x4ProjMatrix.m22() = q;
			m_f4x4ProjMatrix.m23() = qn;
			m_f4x4ProjMatrix.m33() = 1;
		} // ortho
	} // !mCustomProjMatrix

#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
    // Deal with orientation mode
    m_f4x4ProjMatrix = m_f4x4ProjMatrix * 
		KtQuaternion<float>(KMathf::Deg2Rad(m_eOrientationMode*90.f), float3::UNIT_Z);
#endif

	RenderSystem* renderSystem = Root::getSingleton().getRenderSystem();
	// API specific
	renderSystem->_convertProjectionMatrix(m_f4x4ProjMatrix, m_f4x4ProjMatrixRS);
	// API specific for Gpu Programs
	renderSystem->_convertProjectionMatrix(m_f4x4ProjMatrix, m_f4x4ProjMatrixRSDepth, true);


	// Calculate bounding box (local)
	// Box is from 0, down -Z, max dimensions as determined from far plane
	// If infinite view frustum just pick a far value
	float farDist = (m_fFarDist == 0) ? 100000 : m_fFarDist;
	// Near plane bounds
	float3 min(left, bottom, -farDist);
	float3 max(right, top, 0);

	if(m_bCustomProjMatrix)
	{
		// Some custom projection matrices can have unusual inverted settings
		// So make sure the AABB is the right way around to start with
		float3 tmp = min;
		min.makeFloor(max);
		max.makeCeil(tmp);
	}

	if(m_eProjectionType == KPT_PERSPECTIVE)
	{
		// Merge with far plane bounds
		float radio = farDist / m_fNearDist;
		min.makeFloor(Vector3(left * radio, bottom * radio, -farDist));
		max.makeCeil(Vector3(right * radio, top * radio, 0));
	}
	mBoundingBox.setExtents(min, max);

	m_bRecalcFrustum = false;

	// Signal to update frustum clipping planes
	m_bRecalcFrustumPlanes = true;
}

void KmFrustum::UpdateViewImpl(void) const
{

}

void KmFrustum::UpdateFrustumPlanes(void) const
{

}

void KmFrustum::UpdateFrustumPlanesImpl(void) const
{

}

void KmFrustum::UpdateWorldSpaceCorners(void) const
{

}

void KmFrustum::UpdateWorldSpaceCornersImpl(void) const
{

}

void KmFrustum::UpdateVertexData(void) const
{
	if(m_bRecalcVertexData)
	{
		if(m_kpVertexData.m_pkVertexBufferBinding->GetBufferCount() <= 0)
		{
			// Initialise vertex & index data
			m_kpVertexData.m_pkVertexDeclaration->AddElement(0, 0, VET_FLOAT3, VES_POSITION);
			mVertexData.vertexCount = 32;
			mVertexData.vertexStart = 0;
			mVertexData.vertexBufferBinding->setBinding( 0,
				HardwareBufferManager::getSingleton().createVertexBuffer(
					sizeof(float)*3, 32, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY) );
		}

		// Note: Even though we can dealing with general projection matrix here,
		//       but because it's incompatibly with infinite far plane, thus, we
		//       still need to working with projection parameters.

		// Calc near plane corners
		Real vpLeft, vpRight, vpBottom, vpTop;
		calcProjectionParameters(vpLeft, vpRight, vpBottom, vpTop);

		// Treat infinite fardist as some arbitrary far value
		Real farDist = (mFarDist == 0) ? 100000 : mFarDist;

		// Calc far plane corners
		Real radio = mProjType == PT_PERSPECTIVE ? farDist / mNearDist : 1;
		Real farLeft = vpLeft * radio;
		Real farRight = vpRight * radio;
		Real farBottom = vpBottom * radio;
		Real farTop = vpTop * radio;

		// Calculate vertex positions (local)
		// 0 is the origin
		// 1, 2, 3, 4 are the points on the near plane, top left first, clockwise
		// 5, 6, 7, 8 are the points on the far plane, top left first, clockwise
		HardwareVertexBufferSharedPtr vbuf = mVertexData.vertexBufferBinding->getBuffer(0);
		float* pFloat = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		// near plane (remember frustum is going in -Z direction)
		*pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
		*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

		*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
		*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

		*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
		*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

		*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
		*pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

		// far plane (remember frustum is going in -Z direction)
		*pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;
		*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

		*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;
		*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

		*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;
		*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;

		*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;
		*pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;

		// Sides of the pyramid
		*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
		*pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearDist;

		*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
		*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

		*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
		*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

		*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
		*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

		// Sides of the box

		*pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearDist;
		*pFloat++ = farLeft;  *pFloat++ = farTop;  *pFloat++ = -farDist;

		*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
		*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

		*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
		*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

		*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
		*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;


		vbuf->unlock();

		mRecalcVertexData = false;
	}
}

bool KmFrustum::IsViewOutOfDate(void) const
{

}

bool KmFrustum::IsFrustumOutOfDate(void) const
{
	// Deriving custom near plane from linked plane?
	if(m_bObliqueDepthProjection)
	{
		// Out of date when view out of data since plane needs to be in view space
		if(IsViewOutOfDate())
		{
			m_bRecalcFrustum = true;
		}
		// Update derived plane
		K_ASSERT(false);
/*		if(m_pkLinkedObliqueProjPlane && 
			!(m_kLastLinkedObliqueProjPlane == m_pkLinkedObliqueProjPlane->_GetDerivedPlane()))
		{
			m_kObliqueProjPlane = m_pkLinkedObliqueProjPlane->_GetDerivedPlane();
			m_pkLastLinkedObliqueProjPlane = m_kObliqueProjPlane;
			m_bRecalcFrustum = true;
		}*/
	}

	return m_bRecalcFrustum;
}

void KmFrustum::InvalidateFrustum(void) const
{
	m_bRecalcFrustum = true;
	m_bRecalcFrustumPlanes = true;
	m_bRecalcWorldSpaceCorners = true;
	m_bRecalcVertexData = true;
}

void KmFrustum::InvalidateView(void) const
{
	m_bRecalcView = true;
	m_bRecalcFrustumPlanes = true;
	m_bRecalcWorldSpaceCorners = true;
}
