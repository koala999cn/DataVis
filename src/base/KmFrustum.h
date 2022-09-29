#pragma once
#include "Klym.h"
#include "KtMatrix4.h"
#include "KtPlane.h"
#include "KpVertexData.h"

typedef KtPlane<float> KPlanef;

class KcCamera;
class KgRenderQueue;
class KpRenderOperation;
class KgMovablePlane;

 #pragma warning(disable : 4251)

/** Specifies orientation mode.
*/
enum KeOrientationMode
{
	KOR_DEGREE_0       = 0,
	KOR_DEGREE_90      = 1,
	KOR_DEGREE_180     = 2,
	KOR_DEGREE_270     = 3,

	KOR_PORTRAIT       = KOR_DEGREE_0,
	KOR_LANDSCAPERIGHT = KOR_DEGREE_90,
	KOR_LANDSCAPELEFT  = KOR_DEGREE_270
};

/** Specifies perspective (realistic) or orthographic (architectural) projection.
*/
enum KeProjectionType
{
	KPT_PERSPECTIVE,  // 透视投影
	KPT_ORTHOGRAPHIC  // 正交投影
};

/** Worldspace clipping planes.
*/
enum KeFrustumPlane
{
    KFP_NEAR   = 0,
    KFP_FAR    = 1,
    KFP_LEFT   = 2,
    KFP_RIGHT  = 3,
    KFP_TOP    = 4,
    KFP_BOTTOM = 5
};

/** A frustum represents a pyramid, capped at the near and far end which is
    used to represent either a visible area or a projection area. Can be used
    for a number of applications.
*/
class KLYM_CORE_EXPORT KmFrustum
{
public:
	KmFrustum(void);
	virtual ~KmFrustum(void);

	/** Sets the Y-dimension Field Of View (FOV) of the frustum.
		@remarks
			Field Of View (FOV) is the angle made between the frustum's position, and the edges
			of the 'screen' onto which the scene is projected. High values (90+ degrees) result in a wide-angle,
			fish-eye kind of view, low values (30- degrees) in a stretched, telescopic kind of view. Typical values
			are between 45 and 60 degrees.
		@par
			This value represents the VERTICAL field-of-view. The horizontal field of view is calculated from
			this depending on the dimensions of the viewport (they will only be the same if the viewport is square).
		@note
			Setting the FOV overrides the value supplied for frustum::setNearClipPlane.
		*/
	virtual void SetFovAngleY(float fFovY);

	/** Retrieves the frustums Y-dimension Field Of View (FOV).
	*/
	virtual float GetFovAngleY(void) const;

	/** Sets the position of the near clipping plane.
		@remarks
			The position of the near clipping plane is the distance from the frustums position to the screen
			on which the world is projected. The near plane distance, combined with the field-of-view and the
			aspect ratio, determines the size of the viewport through which the world is viewed (in world
			co-ordinates). Note that this world viewport is different to a screen viewport, which has it's
			dimensions expressed in pixels. The frustums viewport should have the same aspect ratio as the
			screen viewport it renders into to avoid distortion.
		@param
			near The distance to the near clipping plane from the frustum in world coordinates.
		*/
	virtual void SetNearClipDistance(float fNearDist);

	/** Sets the position of the near clipping plane.
	*/
	virtual float getNearClipDistance(void) const;

	/** Sets the distance to the far clipping plane.
		@remarks
			The view frustum is a pyramid created from the frustum position and the edges of the viewport.
			This method sets the distance for the far end of that pyramid. 
			Different applications need different values: e.g. a flight sim
			needs a much further far clipping plane than a first-person 
			shooter. An important point here is that the larger the ratio 
			between near and far clipping planes, the lower the accuracy of
			the Z-buffer used to depth-cue pixels. This is because the
			Z-range is limited to the size of the Z buffer (16 or 32-bit) 
			and the max values must be spread over the gap between near and
			far clip planes. As it happens, you can affect the accuracy far 
			more by altering the near distance rather than the far distance, 
			but keep this in mind.
		@param
			far The distance to the far clipping plane from the frustum in 
			world coordinates.If you specify 0, this means an infinite view
			distance which is useful especially when projecting shadows; but
			be careful not to use a near distance too close.
	*/
	virtual void SetFarClipDistance(float fFarDist);

	/** Retrieves the distance from the frustum to the far clipping plane.
	*/
	virtual float GetFarClipDistance(void) const;

	/** Sets the aspect ratio for the frustum viewport.
		@remarks
			The ratio between the x and y dimensions of the rectangular area visible through the frustum
			is known as aspect ratio: aspect = width / height .
		@par
			The default for most fullscreen windows is 1.3333 - this is also assumed by Ogre unless you
			use this method to state otherwise.
	*/
	virtual void SetAspectRatio(float fRatio);

	/** Retreives the current aspect ratio.
	*/
	virtual float GetAspectRatio(void) const;

	/** Sets frustum offsets, used in stereo rendering.
		@remarks
			You can set both horizontal and vertical plane offsets of "eye"; in
			stereo rendering frustum is moved in horizontal plane. To be able to
			render from two "eyes" you'll need two cameras rendering on two
			RenderTargets.
		@par
			The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
		@param
			offset The horizontal and vertical plane offsets.
	*/
	virtual void SetFrustumOffset(const float offset[2]);

	/** Sets frustum offsets, used in stereo rendering.
		@remarks
			You can set both horizontal and vertical plane offsets of "eye"; in
			stereo rendering frustum is moved in horizontal plane. To be able to
			render from two "eyes" you'll need two cameras rendering on two
			RenderTargets.
		@par
			The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
		@param
			horizontal The horizontal plane offset.
		@param
			vertical The vertical plane offset.
	*/
	virtual void SetFrustumOffset(float horizontal = 0.f, float vertical = 0.f);

	/** Retrieves the frustum offsets.
	*/
	virtual const float* GetFrustumOffset() const;

	/** Sets frustum focal length (used in stereo rendering).
		@param
			focalLength The distance to the focal plane from the frustum in world coordinates.
	*/
	virtual void SetFocalLength(float fFocalLength = 1.f);

	/** Returns focal length of frustum.
	*/
	virtual float GetFocalLength() const;

	/** Manually set the extents of the frustum.
	@param left, right, top, bottom The position where the side clip planes intersect
		the near clip plane, in eye space
	*/
	virtual void SetFrustumExtents(float left, float right, float top, float bottom);
	/** Reset the frustum extents to be automatically derived from other params. */
	virtual void ResetFrustumExtents(); 
	/** Get the extents of the frustum in view space. */
	virtual void GetFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const;


	/** Gets the projection matrix for this frustum adjusted for the current
		rendersystem specifics (may be right or left-handed, depth range
		may vary).
	@remarks
		This method retrieves the rendering-API dependent version of the projection
		matrix. If you want a 'typical' projection matrix then use 
		getProjectionMatrix.

	*/
	virtual const float4x4& GetProjectionMatrixRS(void) const;
	/** Gets the depth-adjusted projection matrix for the current rendersystem,
		but one which still conforms to right-hand rules.
	@remarks
		This differs from the rendering-API dependent getProjectionMatrix
		in that it always returns a right-handed projection matrix result 
		no matter what rendering API is being used - this is required for
		vertex and fragment programs for example. However, the resulting depth
		range may still vary between render systems since D3D uses [0,1] and 
		GL uses [-1,1], and the range must be kept the same between programmable
		and fixed-function pipelines.
	*/
	virtual const float4x4& GetProjectionMatrixWithRSDepth(void) const;
	/** Gets the normal projection matrix for this frustum, ie the 
	projection matrix which conforms to standard right-handed rules and
	uses depth range [-1,+1].
	@remarks
		This differs from the rendering-API dependent getProjectionMatrixRS
		in that it always returns a right-handed projection matrix with depth
		range [-1,+1], result no matter what rendering API is being used - this
		is required for some uniform algebra for example.
	*/
	virtual const float4x4& GetProjectionMatrix(void) const;

	/** Gets the view matrix for this frustum. Mainly for use by OGRE internally.
	*/
	virtual const float4x4& GetViewMatrix(void) const;

	/** Calculate a view matrix for this frustum, relative to a potentially dynamic point. 
		Mainly for use by OGRE internally when using camera-relative rendering
		for frustums that are not the centre (e.g. texture projection)
	*/
	virtual void CalcViewMatrixRelative(const float3& f3RelPos, float4x4& f4x4MatToUpdate) const;

	/** Set whether to use a custom view matrix on this frustum.
	@remarks
		This is an advanced method which allows you to manually set
		the view matrix on this frustum, rather than having it calculate
		itself based on it's position and orientation. 
	@note
		After enabling a custom view matrix, the frustum will no longer
		update on its own based on position / orientation changes. You 
		are completely responsible for keeping the view matrix up to date.
		The custom matrix will be returned from getViewMatrix.
	@param enable If true, the custom view matrix passed as the second 
		parameter will be used in preference to an auto calculated one. If
		false, the frustum will revert to auto calculating the view matrix.
	@param viewMatrix The custom view matrix to use, the matrix must be an
		affine matrix.
	@see Frustum::setCustomProjectionMatrix, float4x4::isAffine
	*/
	virtual void SetCustomViewMatrix(bool bEnable, const float4x4& viewMatrix = float4x4::IDENTITY);
	/// Returns whether a custom view matrix is in use
	virtual bool IsCustomViewMatrixEnabled(void) const { return m_bCustomViewMatrix; }
		
	/** Set whether to use a custom projection matrix on this frustum.
	@remarks
		This is an advanced method which allows you to manually set
		the projection matrix on this frustum, rather than having it 
		calculate itself based on it's position and orientation. 
	@note
		After enabling a custom projection matrix, the frustum will no 
		longer update on its own based on field of view and near / far
		distance changes. You are completely responsible for keeping the 
		projection matrix up to date if those values change. The custom 
		matrix will be returned from getProjectionMatrix and derivative
		functions.
	@param enable If true, the custom projection matrix passed as the 
		second parameter will be used in preference to an auto calculated 
		one. If	false, the frustum will revert to auto calculating the 
		projection matrix.
	@param projectionMatrix The custom view matrix to use
	@see Frustum::setCustomViewMatrix
	*/
	virtual void SetCustomProjectionMatrix(bool bEnable, const float4x4& projectionMatrix = float4x4::IDENTITY);
	/// Returns whether a custom projection matrix is in use
	virtual bool IsCustomProjectionMatrixEnabled(void) const { return m_bCustomProjMatrix; }

	/** Retrieves the clipping planes of the frustum (world space).
	@remarks
		The clipping planes are ordered as declared in enumerate constants FrustumPlane.
	*/
	virtual const KPlanef* GetFrustumPlanes(void) const;

	/** Retrieves a specified plane of the frustum (world space).
		@remarks
			Gets a reference to one of the planes which make up the frustum frustum, e.g. for clipping purposes.
	*/
	virtual const KPlanef& GetFrustumPlane( unsigned short plane ) const;

	/** Tests whether the given container is visible in the Frustum.
		@param
			bound Bounding box to be checked (world space)
		@param
			culledBy Optional pointer to an int which will be filled by the plane number which culled
			the box if the result was false;
		@return
			If the box was visible, true is returned.
		@par
			Otherwise, false is returned.
	*/
//	virtual bool IsVisible(const KtAxisAlignedBox<float>& bound, FrustumPlane* culledBy = 0) const;

	/** Tests whether the given container is visible in the Frustum.
		@param
			bound Bounding sphere to be checked (world space)
		@param
			culledBy Optional pointer to an int which will be filled by the plane number which culled
			the box if the result was false;
		@return
			If the sphere was visible, true is returned.
		@par
			Otherwise, false is returned.
	*/
//	virtual bool IsVisible(const KtSphere<float>& bound, FrustumPlane* culledBy = 0) const;

	/** Tests whether the given vertex is visible in the Frustum.
		@param
			vert Vertex to be checked (world space)
		@param
			culledBy Optional pointer to an int which will be filled by the plane number which culled
			the box if the result was false;
		@return
			If the box was visible, true is returned.
		@par
			Otherwise, false is returned.
	*/
//	virtual bool IsVisible(const float3& vert, FrustumPlane* culledBy = 0) const;

	/// Overridden from MovableObject::getTypeFlags
	unsigned int GetTypeFlags(void) const;

	/** Overridden from MovableObject */
	const KtAxisAlignedBox<float>& GetBoundingBox(void) const;

	/** Overridden from MovableObject */
	float GetBoundingRadius(void) const;

	/** Overridden from MovableObject */
	void _UpdateRenderQueue(KgRenderQueue* queue);

	/** Overridden from MovableObject */
//	const KcString& GetMovableType(void) const;

	/** Overridden from MovableObject */
	void _NotifyCurrentCamera(KcCamera* pkCamera);

	/** Overridden from Renderable */
//	const MaterialPtr& getMaterial(void) const;

	/** Overridden from Renderable */
	void GetRenderOperation(KpRenderOperation& op);

	/** Overridden from Renderable */
	void GetWorldTransforms(float4x4* xform) const;

	/** Overridden from Renderable */
	float GetSquaredViewDepth(const KcCamera* cam) const;

	/** Overridden from Renderable */
//	const KLightList& GetLights(void) const;

	/** Gets the world space corners of the frustum.
	@remarks
		The corners are ordered as follows: top-right near, 
		top-left near, bottom-left near, bottom-right near, 
		top-right far, top-left far, bottom-left far, bottom-right far.
	*/
	virtual const float3* GetWorldSpaceCorners(void) const;

	/** Sets the type of projection to use (orthographic or perspective). Default is perspective.
	*/
	virtual void SetProjectionType(KeProjectionType pt);

	/** Retrieves info on the type of projection used (orthographic or perspective).
	*/
	virtual KeProjectionType GetProjectionType(void) const;

	/** Sets the orthographic window settings, for use with orthographic rendering only. 
	@note Calling this method will recalculate the aspect ratio, use 
		setOrthoWindowHeight or setOrthoWindowWidth alone if you wish to 
		preserve the aspect ratio but just fit one or other dimension to a 
		particular size.
	@param w, h The dimensions of the view window in world units
	*/
	virtual void SetOrthoWindow(float w, float h);
	/** Sets the orthographic window height, for use with orthographic rendering only. 
	@note The width of the window will be calculated from the aspect ratio. 
	@param h The height of the view window in world units
	*/
	virtual void SetOrthoWindowHeight(float h);
	/** Sets the orthographic window width, for use with orthographic rendering only. 
	@note The height of the window will be calculated from the aspect ratio. 
	@param w The width of the view window in world units
	*/
	virtual void SetOrthoWindowWidth(float w);
	/** Gets the orthographic window height, for use with orthographic rendering only. 
	*/
	virtual float GetOrthoWindowHeight() const;
	/** Gets the orthographic window width, for use with orthographic rendering only. 
	@note This is calculated from the orthographic height and the aspect ratio
	*/
	virtual float GetOrthoWindowWidth() const;

	/** Modifies this frustum so it always renders from the reflection of itself through the
	plane specified.
	@remarks
	This is obviously useful for performing planar reflections. 
	*/
	virtual void EnableReflection(const KPlanef& p);
	/** Modifies this frustum so it always renders from the reflection of itself through the
	plane specified. Note that this version of the method links to a plane
	so that changes to it are picked up automatically. It is important that
	this plane continues to exist whilst this object does; do not destroy
	the plane before the frustum.
	@remarks
	This is obviously useful for performing planar reflections. 
	*/
//	virtual void EnableReflection(const MovablePlane* p);

	/** Disables reflection modification previously turned on with enableReflection */
	virtual void DisableReflection(void);

	/// Returns whether this frustum is being reflected
	virtual bool IsReflected(void) const { return m_bReflect; }
	/// Returns the reflection matrix of the frustum if appropriate
	virtual const float4x4& GetReflectionMatrix(void) const { return m_f4x4ReflectMatrix; }
	/// Returns the reflection plane of the frustum if appropriate
	virtual const KPlanef& GetReflectionPlane(void) const { return m_kReflectPlane; }

	/** Project a sphere onto the near plane and get the bounding rectangle. 
	@param sphere The world-space sphere to project
	@param radius Radius of the sphere
	@param left, top, right, bottom Pointers to destination values, these
		will be completed with the normalised device coordinates (in the 
		range {-1,1})
	@return true if the sphere was projected to a subset of the near plane,
		false if the entire near plane was contained
	*/
//	virtual bool ProjectSphere(const KSpheref& sphere, 
//		float* left, float* top, float* right, float* bottom) const;


	/** Links the frustum to a custom near clip plane, which can be used
		to clip geometry in a custom manner without using user clip planes.
	@remarks
		There are several applications for clipping a scene arbitrarily by
		a single plane; the most common is when rendering a reflection to 
		a texture, and you only want to render geometry that is above the 
		water plane (to do otherwise results in artefacts). Whilst it is
		possible to use user clip planes, they are not supported on all
		cards, and sometimes are not hardware accelerated when they are
		available. Instead, where a single clip plane is involved, this
		technique uses a 'fudging' of the near clip plane, which is 
		available and fast on all hardware, to perform as the arbitrary
		clip plane. This does change the shape of the frustum, leading 
		to some depth buffer loss of precision, but for many of the uses of
		this technique that is not an issue.
	@par 
		This version of the method links to a plane, rather than requiring
		a by-value plane definition, and therefore you can 
		make changes to the plane (e.g. by moving / rotating the node it is
		attached to) and they will automatically affect this object.
	@note This technique only works for perspective projection.
	@param plane The plane to link to to perform the clipping. This plane
		must continue to exist while the camera is linked to it; do not
		destroy it before the frustum. 
	*/
//	virtual void EnableCustomNearClipPlane(const MovablePlane* plane);
	/** Links the frustum to a custom near clip plane, which can be used
		to clip geometry in a custom manner without using user clip planes.
	@remarks
		There are several applications for clipping a scene arbitrarily by
		a single plane; the most common is when rendering a reflection to  
		a texture, and you only want to render geometry that is above the 
		water plane (to do otherwise results in artefacts). Whilst it is
		possible to use user clip planes, they are not supported on all
		cards, and sometimes are not hardware accelerated when they are
		available. Instead, where a single clip plane is involved, this
		technique uses a 'fudging' of the near clip plane, which is 
		available and fast on all hardware, to perform as the arbitrary
		clip plane. This does change the shape of the frustum, leading 
		to some depth buffer loss of precision, but for many of the uses of
		this technique that is not an issue.
	@note This technique only works for perspective projection.
	@param plane The plane to link to to perform the clipping. This plane
		must continue to exist while the camera is linked to it; do not
		destroy it before the frustum. 
	*/
	virtual void EnableCustomNearClipPlane(const KPlanef& plane);
	/** Disables any custom near clip plane. */
	virtual void DisableCustomNearClipPlane(void);
	/** Is a custom near clip plane in use? */
	virtual bool IsCustomNearClipPlaneEnabled(void) const { return m_bObliqueDepthProjection; }

	/// @copydoc MovableObject::visitRenderables
//	void VisitRenderables(Renderable::Visitor* visitor, bool debugRenderables = false);

	/// Small constant used to reduce far plane projection to avoid inaccuracies
	static const float INFINITE_FAR_PLANE_ADJUST;

	/** Get the derived position of this frustum. */
	virtual const float3& GetPositionForViewUpdate(void) const;
	/** Get the derived orientation of this frustum. */
	virtual const KQuaternionf& GetOrientationForViewUpdate(void) const;

	/** Gets a world-space list of planes enclosing the frustum.
	*/
//	PlaneBoundedVolume getPlaneBoundedVolume();
	/** Set the orientation mode of the frustum. Default is OR_DEGREE_0
			@remarks
			Setting the orientation of a frustum is only supported on
			iOS at this time.  An exception is thrown on other platforms.
	*/
	void SetOrientationMode(KeOrientationMode eMode);

	/** Get the orientation mode of the frustum.
			@remarks
			Getting the orientation of a frustum is only supported on
			iOS at this time.  An exception is thrown on other platforms.
	*/
	KeOrientationMode GetOrientationMode() const;

protected:
	// Internal functions for calcs
	virtual void CalcProjectionParameters(float& left, float& right, float& bottom, float& top) const;
	/// Update frustum if out of date
	virtual void UpdateFrustum(void) const;
	/// Update view if out of date
	virtual void UpdateView(void) const;
	/// Implementation of updateFrustum (called if out of date)
	virtual void UpdateFrustumImpl(void) const;
	/// Implementation of updateView (called if out of date)
	virtual void UpdateViewImpl(void) const;
	virtual void UpdateFrustumPlanes(void) const;
	/// Implementation of updateFrustumPlanes (called if out of date)
	virtual void UpdateFrustumPlanesImpl(void) const;
	virtual void UpdateWorldSpaceCorners(void) const;
	/// Implementation of updateWorldSpaceCorners (called if out of date)
	virtual void UpdateWorldSpaceCornersImpl(void) const;
	virtual void UpdateVertexData(void) const;
	virtual bool IsViewOutOfDate(void) const;
	virtual bool IsFrustumOutOfDate(void) const;
	/// Signal to update frustum information.
	virtual void InvalidateFrustum(void) const;
	/// Signal to update view information.
	virtual void InvalidateView(void) const;

private:
	// 相机的投影类型：正交投影还是投射投影？
	KeProjectionType m_eProjectionType;

    /// y-direction field-of-view (default 45 degree)
	/// y方向的视场
    float m_fFOVy; // 弧度

    /// Far clip distance - default 10000
    float m_fFarDist;

    /// Near clip distance - default 100
    float m_fNearDist;

    /// x/y KgViewport ratio - default 1.3333
    float m_fAspectRatio;

	/// Ortho height size (world units)
	float m_fOrthoHeight;

	/// Off-axis frustum center offset - default (0.0, 0.0)
	float m_f2FrustumOffset[2];

	/// Focal length of frustum (for stereo rendering, defaults to 1.0)
	float m_fFocalLength;

	/// The 6 main clipping planes
	mutable KPlanef m_k6FrustumPlanes[6];

	/// Stored versions of parent orientation / position
	mutable KtQuaternion<float> m_kLastParentOrientation;
	mutable float3 m_f3LastParentPosition;

	/// Pre-calced projection matrix for the specific render system
	mutable float4x4 m_f4x4ProjMatrixRS;
	/// Pre-calced standard projection matrix but with render system depth range
	mutable float4x4 m_f4x4ProjMatrixRSDepth;
	/// Pre-calced standard projection matrix
	mutable float4x4 m_f4x4ProjMatrix;
	/// Pre-calced view matrix
	mutable float4x4 m_f4x4ViewMatrix;
	/// Something's changed in the frustum shape?
	mutable bool m_bRecalcFrustum;
	/// Something re the view pos has changed
	mutable bool m_bRecalcView;
	/// Something re the frustum planes has changed
	mutable bool m_bRecalcFrustumPlanes;
	/// Something re the world space corners has changed
	mutable bool m_bRecalcWorldSpaceCorners;
	/// Something re the vertex data has changed
	mutable bool m_bRecalcVertexData;
	/// Are we using a custom view matrix?
	bool m_bCustomViewMatrix;
	/// Are we using a custom projection matrix?
	bool m_bCustomProjMatrix;
	/// Have the frustum extents been manually set?
	bool m_bFrustumExtentsManuallySet;
	/// Frustum extents
	mutable float m_fLeft, m_fRight, m_fTop, m_fBottom;
	/// Frustum orientation mode
	mutable KeOrientationMode m_eOrientationMode;

	// TODO: remove it
	bool m_bNeedUpdate; // 是否需要更新m_f4ProjMatrix

	/// Shared class-level name for Movable type
//	static KcString ms_strMovableType;

	mutable KtAxisAlignedBox<float> m_kBoundingBox;
	mutable KpVertexData m_kpVertexData;

//	MaterialPtr mMaterial;
	mutable float3 m_f3WorldSpaceCorners[8];

	/// Is this frustum to act as a reflection of itself?
	bool m_bReflect;
	/// Derived reflection matrix
	mutable float4x4 m_f4x4ReflectMatrix;
	/// Fixed reflection plane
	mutable KPlanef m_kReflectPlane;
	/// Pointer to a reflection plane (automatically updated)
	const KgMovablePlane* m_pkLinkedReflectPlane;
	/// Record of the last world-space reflection plane info used
	mutable KPlanef m_kLastLinkedReflectionPlane;
		
	/// Is this frustum using an oblique depth projection?
	bool m_bObliqueDepthProjection;
	/// Fixed oblique projection plane
	mutable KPlanef m_kObliqueProjPlane;
	/// Pointer to oblique projection plane (automatically updated)
	const KgMovablePlane* m_pkLinkedObliqueProjPlane;
	/// Record of the last world-space oblique depth projection plane info used
	mutable KPlanef m_kLastLinkedObliqueProjPlane;
};

