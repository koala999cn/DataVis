#pragma once
#include "KiMovableObject.h"
#include "KtVector3.h"
#include "KtMatrix4.h"
#include "KmFrustum.h"
#include "KtWeakPtr.h"

class KcSceneManager;

/** The polygon mode to use when rasterising. */
enum KePolygonMode // TODO: move to other place
{
	/// Only points are rendered.
    KPM_POINTS = 1,
	/// Wireframe models are rendered.
    KPM_WIREFRAME = 2,
	/// Solid polygons are rendered.
    KPM_SOLID = 3
};

/** A viewpoint from which the scene will be rendered.
    @remarks
        OGRE renders scenes from a camera viewpoint into a buffer of
        some sort, normally a window or a texture (a subclass of
        RenderTarget). OGRE cameras support both perspective projection (the default,
        meaning objects get smaller the further away they are) and
        orthographic projection (blueprint-style, no decrease in size
        with distance). Each camera carries with it a style of rendering,
        e.g. full textured, flat shaded, wireframe), field of view,
        rendering distances etc, allowing you to use OGRE to create
        complex multi-window views if required. In addition, more than
        one camera can point at a single render target if required,
        each rendering to a subset of the target, allowing split screen
        and picture-in-picture views.
    @par
        Cameras maintain their own aspect ratios, field of view, and frustum,
        and project co-ordinates into a space measured from -1 to 1 in x and y,
        and 0 to 1 in z. At render time, the camera will be rendering to a
        Viewport which will translate these parametric co-ordinates into real screen
        co-ordinates. Obviously it is advisable that the viewport has the same
        aspect ratio as the camera to avoid distortion (unless you want it!).
    @par
        Note that a Camera can be attached to a SceneNode, using the method
        SceneNode::attachObject. If this is done the Camera will combine it's own
        position/orientation settings with it's parent SceneNode. 
        This is useful for implementing more complex Camera / object
        relationships i.e. having a camera attached to a world object.
*/
class KLYM_CORE_EXPORT KcCamera : public KiMovableObject, public KmFrustum
{
public:
	/** Standard constructor.
	*/
	KcCamera(const KcString& ksName, KtWeakPtr<KcSceneManager> pkSM);

	/** Standard destructor.
	*/
	virtual ~KcCamera(void);

	// 调整摄像机位置、方位
	void LookAt(float3& f3FocusPos); // 保持位置等参数不变，旋转摄像机，以对准f3FocusPos
//	void Show(const KcAABB& kBound, float fRatio = 1.f); // 保持宽高比、视场不变，调整摄像机的位置、方位，以在屏幕上按fRatio比例显示kBox，必要时自动修正Z的远近值

	// 获取属性参数
	float3 GetRight();
	float3 GetUp();
	float3 GetDirection();
	const float4x4& GetViewMatrix();

protected:

	/// Scene manager responsible for the scene
	KtWeakPtr<KcSceneManager> m_pkSceneManager;

	/// Camera orientation, quaternion style
	KtQuaternion<float> m_kOrientation;

	/// Camera position - default (0,0,0)
	float3 m_f3Position;

	/// Derived orientation/position of the camera, including reflection
	mutable KtQuaternion<float> m_kDerivedOrientation;
	mutable float3 m_f3DerivedPosition;

	/// Real world orientation/position of the camera
	mutable KtQuaternion<float> m_kRealOrientation;
	mutable float3 m_f3RealPosition;

	/// Whether to yaw around a fixed axis.
	bool m_bYawFixed;
	/// Fixed axis to yaw around
	float3 m_f3YawFixedAxis;

	/// Rendering type
	KePolygonMode m_eSceneDetail;

	/// Stored number of visible faces in the last render
	unsigned int m_nVisFacesLastRender;

	/// Stored number of visible faces in the last render
	unsigned int m_nVisBatchesLastRender;

	/// Shared class-level name for Movable type
	static KcString ms_strMovableType;

	/// SceneNode which this Camera will automatically track
	KcSceneNode* m_pkAutoTrackTarget;
	/// Tracking offset for fine tuning
//	Vectofloat3r3 mAutoTrackOffset;

	// Scene LOD factor used to adjust overall LOD
	float m_fSceneLodFactor;
	/// Inverted scene LOD factor, can be used by Renderables to adjust their LOD
	float m_fSceneLodFactorInv;


	/** Viewing window. 
	@remarks
	Generalize camera class for the case, when viewing frustum doesn't cover all viewport.
	*/
	float m_fWLeft, m_fWTop, m_fWRight, m_fWBottom;
	/// Is viewing window used.
	bool m_bWindowSet;
	/// Windowed viewport clip planes 
//	mutable vector<Plane>::type mWindowClipPlanes;
	// Was viewing window changed.
	mutable bool m_bRecalcWindow;
	/// The last viewport to be added using this camera
	KgViewport* m_pkLastViewport;
	/** Whether aspect ratio will automatically be recalculated 
		when a viewport changes its size
	*/
	bool m_bAutoAspectRatio;
	/// Custom culling frustum
	KmFrustum *m_pkCullFrustum;
	/// Whether or not the rendering distance of objects should take effect for this camera
	bool m_bUseRenderingDistance;
	/// Camera to use for LOD calculation
	const KcCamera* m_pkLodCamera;
		
	/// Whether or not the minimum display size of objects should take effect for this camera
	bool m_bUseMinPixelSize;
	/// @see Camera::getPixelDisplayRatio
	float m_fPixelDisplayRatio;
};
