#include "KcCamera.h"
#include "KcSceneManager.h"

KcCamera::KcCamera(const KcString& ksName, KtWeakPtr<KcSceneManager> pkSM) : 
		m_pkSceneManager(pkSM), 
		KiMovableObject(ksName),
		m_kOrientation(KtQuaternion<float>::IDENTITY),
		m_f3Position(float3::ZERO),
		m_eSceneDetail(KPM_SOLID),
		m_pkAutoTrackTarget(0),
//		mAutoTrackOffset(Vector3::ZERO),
		m_fSceneLodFactor(1.f),
		m_fSceneLodFactorInv(1.f),
		m_bWindowSet(false),
		m_pkLastViewport(0),
		m_bAutoAspectRatio(false),
		m_pkCullFrustum(0),
		m_bUseRenderingDistance(true),
		m_pkLodCamera(0),
		m_bUseMinPixelSize(false),
		m_fPixelDisplayRatio(0.f)
{
	// Reasonable defaults to camera params
	mFOVy = Radian(Math::PI/4.0f);
	mNearDist = 100.0f;
	mFarDist = 100000.0f;
	mAspect = 1.33333333333333f;
	mProjType = PT_PERSPECTIVE;
	setFixedYawAxis(true);    // Default to fixed yaw, like freelook since most people expect this

	invalidateFrustum();
	invalidateView();

	// Init matrices
	mViewMatrix = Matrix4::ZERO;
	mProjMatrixRS = Matrix4::ZERO;

	m_pkParentNode = 0;

	// no reflection
	m_bReflect = false;

	m_bVisible = false;
}

KcCamera::~KcCamera(void)
{
//	if(m_pkScene) m_pkScene->Release();
}

float3 KcCamera::GetRight()
{
//	return GetOrientation()*float3::UNIT_X;
}

float3 KcCamera::GetUp()
{
//	return GetOrientation()*float3::UNIT_Y;
}

float3 KcCamera::GetDirection()
{
//	return GetOrientation()*float3::FORWARD;
}

const float4x4& KcCamera::GetViewMatrix()
{
/*	if(KmMovable::IsMoved())
	{// update view matrix

		// View matrix is:
		//
		//  [ Lx  Uy  Dz  Tx  ]
		//  [ Lx  Uy  Dz  Ty  ]
		//  [ Lx  Uy  Dz  Tz  ]
		//  [ 0   0   0   1   ]
		//
		// Where T = -(Transposed(f3Rot) * Pos)

		// This is most efficiently done using 3x3 Matrices
		float3x3 f3Rot;
		KmMovable::GetOrientation().ToRotationMatrix(f3Rot);

		// Make the translation relative to new axes
		f3Rot.Transpose();
		float3 f3Trans = - (f3Rot * KmMovable::GetPosition());

		// Make final matrix
		m_f4View = float4x4::IDENTITY;
		m_f4View = f3Rot; // fills upper 3x3
		m_f4View.m03() = f3Trans.x;
		m_f4View.m13() = f3Trans.y;
		m_f4View.m23() = f3Trans.z;

		KmMovable::ClearMovedFlag();
	}

	return m_f4View;*/
}

void KcCamera::LookAt(float3& f3FocusPos)
{
/*	AimAt(float3::UNIT_Z, f3FocusPos);
	float3 f3Up = GetUp();
	if(f3Up.y < 0)
		this->RotateInLocal(float3::UNIT_Z, KtuMath<float>::PI);

	return;*/

/*	float3 f3Dir = f3FocusPos-GetPosition();
	f3Dir.Normalize();
	float3 f3Up = float3(0,1,0);
	float3 f3Right = f3Up.Cross(f3Dir);
	f3Up = f3Dir.Cross(f3Right);
	KtQuaternion<float> q;
	q.FormAxes(f3Right, f3Up, f3Dir);
	Rotate(q);*/
}

void KcCamera::Show(const KcAABB& kBound, float fRatio)
{
/*	float3 f3Center = (kBound.GetMin()+kBound.GetMax())/2;
	float fLength = (kBound.GetMin()-kBound.GetMax()).Length();
	float3 f3EyePos;//  = f3Center+fLength;
//	this->SetPosition(f3EyePos);
	this->LookAt(f3Center);
	
	// 调整摄像机的位置
	const float4x4& f4View = this->GetViewMatrix();
	const float4x4& f4Proj = this->GetProjectionMatrixLH();
	float3 f3Max = f4View*f4Proj*kBound.GetMax();

	// TODO: 支持非对称相机
	// TODO: 应该需要知道视窗大小
//	(fRatio*GetRight()-f3Max.x);
//	(fRatio*GetTop()-f3Max.y);

//	f3EyePos += (f3EyePos-f3Center)*(fRatio-KtuMath<float>::Min(f3Max.x, f3Max.y));
//	this->SetPosition(f3EyePos);

	// 修正NearZ
	f3EyePos = GetPosition();
	float fMaxNearZ = KtuMath<float>::Abs((f3EyePos-f3Center).Length()-fLength);
	if(GetNearZ() > fMaxNearZ)
		SetNearZ(fMaxNearZ);*/
}
