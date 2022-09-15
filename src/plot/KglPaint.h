#pragma once
#include <vlVG/link_config.hpp>
#include <vlCore/Image.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Text.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/SceneManager.hpp>
#include <vlGraphics/Clear.hpp>
#include <vlGraphics/Scissor.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/FontManager.hpp>
#include "KtVector3.h"
#include "KtVector4.h"


// OpenGL绘图接口
// 从vl::VectorGraphics改编，以支持3d绘图

struct KpLine
{
    int style; // 线型
    double width; // 线宽
    KtVector4<double> color;
};

class KglPaint : public vl::Object
{
public:
    VL_INSTRUMENT_CLASS(KglPaint, Object)

public:
    using vec3 = KtVector3<double>;
    using vec4 = KtVector4<double>;

    //! Defines how the texture is applied to the rendering primitive
    enum ETextureMode
    {
        //! The texture is stretched over the primitive
        TextureMode_Clamp,
        //! The texture is repeated over the primitive
        TextureMode_Repeat
    };

    //! Poligon stipple patterns
    enum EPolygonStipple
    {
        //! The poligon is completely filled (default)
        PolygonStipple_Solid,
        PolygonStipple_Dot,
        PolygonStipple_Chain,
        PolygonStipple_HLine,
        PolygonStipple_VLine
    } ;

    //! Line stipple patterns
    enum ELineStipple
    {
        //! The line is completely filled  (default)
        LineStipple_Solid,
        LineStipple_Dot,
        LineStipple_Dash,
        LineStipple_Dash4,
        LineStipple_Dash8,
        LineStipple_DashDot,
        LineStipple_DashDotDot
    };

private:
    //------------------------------------------------------------------------- start internal
    //! \internal
    class ImageState
    {
    public:
        ImageState(const vl::Image* img, ETextureMode mode) : mImage(img), mTextureMode(mode) {}

        bool operator<(const ImageState& other) const
        {
            if (mImage != other.mImage)
                return mImage < other.mImage;
            else
                if (mTextureMode != other.mTextureMode)
                    return mTextureMode < other.mTextureMode;
                else
                    return false;
        }
    protected:
        const vl::Image* mImage;
        ETextureMode mTextureMode;
    };
    //------------------------------------------------------------------------- start internal
    //! \internal
    class State
    {
    public:
        State()
        {
            using namespace vl;

            mColor = white;
            mPointSize = 5;
            mImage = NULL;
            mTextureMode = TextureMode_Clamp;
            mLogicOp = LO_COPY;
            mPointSmoothing = true;
            mLineSmoothing = true;
            mPolygonSmoothing = false;
            mLineWidth = 1.0;
            mLineStipple = 0xFFFF;
            memset(mPolyStipple, 0xFF, 32 * 32 / 8);

            // blend equation
            mBlendEquationRGB = BE_FUNC_ADD;
            mBlendEquationAlpha = BE_FUNC_ADD;
            // blend factor
            mBlendFactorSrcRGB = BF_SRC_ALPHA;
            mBlendFactorDstRGB = BF_ONE_MINUS_SRC_ALPHA;
            mBlendFactorSrcAlpha = BF_SRC_ALPHA;
            mBlendFactorDstAlpha = BF_ONE_MINUS_SRC_ALPHA;
            // alpha func
            mAlphaFuncRefValue = 0.0f;
            mAlphaFunc = FU_ALWAYS;
            // font
            mFont = defFontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 10, false);
            // masks
            /*mDepthMask   = true;*/
            mColorMask = ivec4(1, 1, 1, 1);
            // stencil
            mStencilMask = 0xFFFFFFFF;
            mStencilTestEnabled = false;
            mStencil_SFail = SO_KEEP;
            mStencil_SFail = SO_KEEP;
            mStencil_DpFail = SO_KEEP;
            mStencil_Function = FU_ALWAYS;
            mStencil_RefValue = 0;
            mStencil_FunctionMask = ~(unsigned int)0;
        }

        vl::fvec4 mColor;
        int mPointSize;
        vl::ref<vl::Image> mImage;
        ETextureMode mTextureMode;
        vl::ELogicOp mLogicOp;
        float mLineWidth;
        bool mPointSmoothing;
        bool mLineSmoothing;
        bool mPolygonSmoothing;
        unsigned short mLineStipple;
        unsigned char mPolyStipple[32 * 32 / 8];
        vl::EBlendEquation mBlendEquationRGB;
        vl::EBlendEquation mBlendEquationAlpha;
        vl::EBlendFactor mBlendFactorSrcRGB;
        vl::EBlendFactor mBlendFactorDstRGB;
        vl::EBlendFactor mBlendFactorSrcAlpha;
        vl::EBlendFactor mBlendFactorDstAlpha;
        float mAlphaFuncRefValue;
        vl::EFunction mAlphaFunc;
        vl::ref<vl::Font> mFont;
        /*bool mDepthMask;*/
        vl::ivec4 mColorMask;
        // stencil
        bool mStencilTestEnabled;
        unsigned int mStencilMask;
        vl::EStencilOp mStencil_SFail;
        vl::EStencilOp mStencil_DpFail;
        vl::EStencilOp mStencil_DpPass;
        vl::EFunction  mStencil_Function;
        int          mStencil_RefValue;
        unsigned int         mStencil_FunctionMask;

        bool operator<(const State& other) const
        {
            // lexicographic sorting
            if (mColor.r() != other.mColor.r())
                return mColor.r() < other.mColor.r();
            else
                if (mColor.g() != other.mColor.g())
                    return mColor.g() < other.mColor.g();
                else
                    if (mColor.b() != other.mColor.b())
                        return mColor.b() < other.mColor.b();
                    else
                        if (mColor.a() != other.mColor.a())
                            return mColor.a() < other.mColor.a();
                        else
                            if (mPointSize != other.mPointSize)
                                return mPointSize < other.mPointSize;
                            else
                                if (mImage != other.mImage)
                                    return mImage < other.mImage;
                                else
                                    if (mTextureMode != other.mTextureMode)
                                        return mTextureMode < other.mTextureMode;
                                    else
                                        if (mPolygonSmoothing != other.mPolygonSmoothing)
                                            return mPolygonSmoothing < other.mPolygonSmoothing;
                                        else
                                            if (mPointSmoothing != other.mPointSmoothing)
                                                return mPointSmoothing < other.mPointSmoothing;
                                            else
                                                if (mLineSmoothing != other.mLineSmoothing)
                                                    return mLineSmoothing < other.mLineSmoothing;
                                                else
                                                    if (mLineWidth != other.mLineWidth)
                                                        return mLineWidth < other.mLineWidth;
                                                    else
                                                        if (mLineStipple != other.mLineStipple)
                                                            return mLineStipple < other.mLineStipple;
                                                        else
                                                            if (mLogicOp != other.mLogicOp)
                                                                return mLogicOp < other.mLogicOp;
                                                            else
                                                                if (memcmp(mPolyStipple, other.mPolyStipple, 32 * 32 / 8) != 0)
                                                                    return memcmp(mPolyStipple, other.mPolyStipple, 32 * 32 / 8) < 0;
                                                                else
                                                                    if (mBlendEquationRGB != other.mBlendEquationRGB)
                                                                        return mBlendEquationRGB < other.mBlendEquationRGB;
                                                                    else
                                                                        if (mBlendEquationAlpha != other.mBlendEquationAlpha)
                                                                            return mBlendEquationAlpha < other.mBlendEquationAlpha;
                                                                        else
                                                                            if (mBlendFactorSrcRGB != other.mBlendFactorSrcRGB)
                                                                                return mBlendFactorSrcRGB < other.mBlendFactorSrcRGB;
                                                                            else
                                                                                if (mBlendFactorDstRGB != other.mBlendFactorDstRGB)
                                                                                    return mBlendFactorDstRGB < other.mBlendFactorDstRGB;
                                                                                else
                                                                                    if (mBlendFactorSrcAlpha != other.mBlendFactorSrcAlpha)
                                                                                        return mBlendFactorSrcAlpha < other.mBlendFactorSrcAlpha;
                                                                                    else
                                                                                        if (mBlendFactorDstAlpha != other.mBlendFactorDstAlpha)
                                                                                            return mBlendFactorDstAlpha < other.mBlendFactorDstAlpha;
                                                                                        else
                                                                                            if (mAlphaFuncRefValue != other.mAlphaFuncRefValue)
                                                                                                return mAlphaFuncRefValue < other.mAlphaFuncRefValue;
                                                                                            else
                                                                                                if (mAlphaFunc != other.mAlphaFunc)
                                                                                                    return mAlphaFunc < other.mAlphaFunc;
                                                                                                else
                                                                                                    if (mFont != other.mFont)
                                                                                                        return mFont < other.mFont;
                                                                                                    else
                                                                                                        /*if ( mDepthMask != other.mDepthMask)
                                                                                                          return mDepthMask < other.mDepthMask;
                                                                                                        else*/
                                                                                                        if (mColorMask.r() != other.mColorMask.r())
                                                                                                            return mColorMask.r() < other.mColorMask.r();
                                                                                                        else
                                                                                                            if (mColorMask.g() != other.mColorMask.g())
                                                                                                                return mColorMask.g() < other.mColorMask.g();
                                                                                                            else
                                                                                                                if (mColorMask.b() != other.mColorMask.b())
                                                                                                                    return mColorMask.b() < other.mColorMask.b();
                                                                                                                else
                                                                                                                    if (mColorMask.a() != other.mColorMask.a())
                                                                                                                        return mColorMask.a() < other.mColorMask.a();
                                                                                                                    else
                                                                                                                        if (mStencilMask != other.mStencilMask)
                                                                                                                            return mStencilMask < other.mStencilMask;
                                                                                                                        else
                                                                                                                            if (mStencilTestEnabled != other.mStencilTestEnabled)
                                                                                                                                return mStencilTestEnabled < other.mStencilTestEnabled;
                                                                                                                            else
                                                                                                                                if (mStencil_SFail != other.mStencil_SFail)
                                                                                                                                    return mStencil_SFail < other.mStencil_SFail;
                                                                                                                                else
                                                                                                                                    if (mStencil_DpFail != other.mStencil_DpFail)
                                                                                                                                        return mStencil_DpFail < other.mStencil_DpFail;
                                                                                                                                    else
                                                                                                                                        if (mStencil_DpPass != other.mStencil_DpPass)
                                                                                                                                            return mStencil_DpPass < other.mStencil_DpPass;
                                                                                                                                        else
                                                                                                                                            if (mStencil_Function != other.mStencil_Function)
                                                                                                                                                return mStencil_Function < other.mStencil_Function;
                                                                                                                                            else
                                                                                                                                                if (mStencil_RefValue != other.mStencil_RefValue)
                                                                                                                                                    return mStencil_RefValue < other.mStencil_RefValue;
                                                                                                                                                else
                                                                                                                                                    if (mStencil_FunctionMask != other.mStencil_FunctionMask)
                                                                                                                                                        return mStencil_FunctionMask < other.mStencil_FunctionMask;
                                                                                                                                                    else
                                                                                                                                                        return false;
        }
    };
    //------------------------------------------------------------------------- end internal

public:
    KglPaint();

    //! Returns the list of Actor[s] generated by a VectorGraphics object.
    const vl::ActorCollection* actors() const { return &mActors; }

    //! Returns the list of Actor[s] generated by a VectorGraphics object.
    vl::ActorCollection* actors() { return &mActors; }

    //! Renders a line starting a point <x1,y1,z1> and ending at point <x2,y2,z2>
    vl::Actor* drawLine(double x1, double y1, double z1, double x2, double y2, double z2);

    //! Renders a line starting a point <x1,y1> and ending at point <x2,y2>
    vl::Actor* drawLine(double x1, double y1, double x2, double y2) {
        return drawLine(x1, y1, 0, x2, y2, 0);
    }

    vl::Actor* drawLine(const vl::dvec2& from, const vl::dvec2& to) {
        return drawLine(from.x(), from.y(), to.x(), to.y());
    }

    vl::Actor* drawLine(const vl::dvec3& from, const vl::dvec3& to) {
        return drawLine(from.x(), from.y(), from.z(), to.x(), to.y(), to.z());
    }

    vl::Actor* drawLine(const vec3& from, const vec3& to) {
        return drawLine(from.x, from.y, from.z, to.x, to.y, to.z);
    }

    //! Renders a set of lines. The 'ln' parameter shoud contain N pairs of dvec2. Each pair defines a line segment.
    vl::Actor* drawLines(const std::vector<vl::dvec2>& ln);

    //! 3d版
    vl::Actor* drawLines(const std::vector<vl::dvec3>& ln);

    //! Renders a line passing through the points defined by 'ln'.
    vl::Actor* drawLineStrip(const std::vector<vl::dvec2>& ln);

    //! 3d版
    vl::Actor* drawLineStrip(const std::vector<vl::dvec3>& ln);

    //! Renders a closed line passing through the points defined by 'ln'.
    vl::Actor* drawLineLoop(const std::vector<vl::dvec2>& ln);

    //! 3d版
    vl::Actor* drawLineLoop(const std::vector<vl::dvec3>& ln);

    //! Renders a convex polygon whose corners are defined by 'poly'
    vl::Actor* fillPolygon(const std::vector<vl::dvec2>& poly);

    //! 3d版
    vl::Actor* fillPolygon(const std::vector<vl::dvec3>& poly);

    //! Renders a set of triangles. The 'triangles' parameters must contain N triplets of dvec2. Each triplet defines a triangle.
    vl::Actor* fillTriangles(const std::vector<vl::dvec2>& triangles);

    //! 3d版
    vl::Actor* fillTriangles(const std::vector<vl::dvec3>& triangles);

    //! Renders a triangle fan.
    vl::Actor* fillTriangleFan(const std::vector<vl::dvec2>& fan);

    //! 3d版
    vl::Actor* fillTriangleFan(const std::vector<vl::dvec3>& fan);

    //! Renders a strip of triangles as defined by the OpenGL primitive GL_TRIANGLE_STRIP.
    vl::Actor* fillTriangleStrip(const std::vector<vl::dvec2>& strip);

    //! 3d版
    vl::Actor* fillTriangleStrip(const std::vector<vl::dvec3>& strip);

    //! Renders a set of rectangles as defined by the OpenGL primitive GL_QUADS
    vl::Actor* fillQuads(const std::vector<vl::dvec2>& quads);

    //! Renders a set of rectangles as defined by the OpenGL primitive GL_QUAD_STRIP
    vl::Actor* fillQuadStrip(const std::vector<vl::dvec2>& quad_strip);

    //! Renders a single point. This is only an utility function. If you want to draw many points use drawPoints(const std::vector<dvec2>& pt) instead.
    vl::Actor* drawPoint(double x, double y);

    //! Renders a set of points using the currently set pointSize(), color() and image().
    vl::Actor* drawPoints(const std::vector<vl::dvec2>& pt);

    //! Renders the outline of an ellipse.
    vl::Actor* drawEllipse(double origx, double origy, double xaxis, double yaxis, int segments = 64);

    //! Renders an ellipse.
    vl::Actor* fillEllipse(double origx, double origy, double xaxis, double yaxis, int segments = 64);

    //! Utility function that renders the outline of a quad.
    vl::Actor* drawQuad(double left, double bottom, double right, double top);

    //! Utility function that renders a single quad.
    vl::Actor* fillQuad(double left, double bottom, double right, double top);

    /** Starts the drawing process. You have to call this function before calling any of the fill* and draw* functions.
     * This function will erase all the previously generated content of the VectorGraphics. */
    void startDrawing() { clear(); }

    /** Continues the rendering on a VectorGraphics object. This function will reset the VectorGraphics state and matrix but will not
     * erase the previously generated graphics. */
    void continueDrawing();

    //! Ends the rendering on a VectorGraphics and releases the resources used during the Actor generation process.
    //! If you intend to continue the rendering or to add new graphics objects later set 'release_cache' to false.
    void endDrawing(bool release_cache = true);

    //! Resets the VectorGraphics removing all the graphics objects and resetting its internal state.
    void clear();

    //! The current color. Note that the current color also modulates the currently active image.
    void setColor(const vl::fvec4& color) { mState.mColor = color; }

    void setColor(const vec4& c) { mState.mColor = vl::fvec4(c.x, c.y, c.z, c.w); }

    //! The current color. Note that the current color also modulates the currently active image.
    const vl::fvec4& color() const { return mState.mColor; }

    //! The current point size
    void setPointSize(int size) { mState.mPointSize = size; }

    //! The current point size
    int pointSize() const { return mState.mPointSize; }

    //! The current image used to texture the rendered objects. Note that the current color also modulates the currently active image.
    void setImage(vl::Image* image) { mState.mImage = image; }

    //! The current image used to texture the rendered objects. Note that the current color also modulates the currently active image.
    const vl::Image* image() const { return mState.mImage.get(); }

    //! The current image used to texture the rendered objects. Note that the current color also modulates the currently active image.
    vl::Image* image() { return mState.mImage.get(); }

    //! Utility function equivalent to 'setImage(image); setPointSize(image->width());'
    void setPoint(vl::Image* image) { setImage(image); setPointSize(image->width()); }

    //! The current texture mode
    void setTextureMode(ETextureMode mode) { mState.mTextureMode = mode; }

    //! The current texture mode
    ETextureMode textureMode() const { return mState.mTextureMode; }

    //! The current logic operation, see also http://www.opengl.org/sdk/docs/man/xhtml/glLogicOp.xml for more information.
    void setLogicOp(vl::ELogicOp op) { mState.mLogicOp = op; }

    //! The current logic operation
    vl::ELogicOp logicOp() const { return mState.mLogicOp; }

    //! The current line width, see also http://www.opengl.org/sdk/docs/man/xhtml/glLineWidth.xml for more information.
    void setLineWidth(float width) { mState.mLineWidth = width; }

    //! The current line width
    float lineWidth() const { return mState.mLineWidth; }

    //! The current point smoothing mode
    void setPointSmoothing(bool smooth) { mState.mPointSmoothing = smooth; }

    //! The current point smoothing mode
    bool pointSmoothing() const { return mState.mPointSmoothing; }

    //! The current line smoothing mode
    void setLineSmoothing(bool smooth) { mState.mLineSmoothing = smooth; }

    //! The current line smoothing mode
    bool lineSmoothing() const { return mState.mLineSmoothing; }

    //! The current polygon smoothing mode
    void setPolygonSmoothing(bool smooth) { mState.mPolygonSmoothing = smooth; }

    //! The current polygon smoothing mode
    bool polygonSmoothing() const { return mState.mPolygonSmoothing; }

    //! The current line stipple, see also http://www.opengl.org/sdk/docs/man/xhtml/glLineStipple.xml for more information.
    void setLineStipple(ELineStipple stipple);

    //! The current line stipple
    void setLineStipple(unsigned short stipple) { mState.mLineStipple = stipple; }

    //! The current line stipple
    unsigned short lineStipple() const { return mState.mLineStipple; }

    //! The current polygon stipple, see also http://www.opengl.org/sdk/docs/man/xhtml/glPolygonStipple.xml for more information.
    void setPolygonStipple(EPolygonStipple stipple);

    //! The current polygon stipple
    void setPolygonStipple(unsigned char* stipple) { memcpy(mState.mPolyStipple, stipple, 32 * 32 / 8); }

    //! The current polygon stipple
    const unsigned char* polygonStipple() const { return mState.mPolyStipple; }

    //! The current polygon stipple
    unsigned char* polygonStipple() { return mState.mPolyStipple; }

    //! The current alpha function, see also http://www.opengl.org/sdk/docs/man/xhtml/glAlphaFunc.xml for more information.
    void setAlphaFunc(vl::EFunction func, float ref_value) { mState.mAlphaFuncRefValue = ref_value; mState.mAlphaFunc = func; }

    //! The current alpha function
    void getAlphaFunc(vl::EFunction& func, float& ref_value) const { ref_value = mState.mAlphaFuncRefValue; func = mState.mAlphaFunc; }

    //! The current blending factor, see also http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml for more information.
    void setBlendFunc(vl::EBlendFactor src_rgb, vl::EBlendFactor dst_rgb, vl::EBlendFactor src_alpha, vl::EBlendFactor dst_alpha);

    //! The current blending factor
    void getBlendFunc(vl::EBlendFactor& src_rgb, vl::EBlendFactor& dst_rgb, vl::EBlendFactor& src_alpha, vl::EBlendFactor& dst_alpha) const;

    //! The current blend equation, see also http://www.opengl.org/sdk/docs/man/xhtml/glBlendEquation.xml for more information.
    void setBlendEquation(vl::EBlendEquation rgb_eq, vl::EBlendEquation alpha_eq);

    //! The current blend equation.
    void getBlendEquation(vl::EBlendEquation& rgb_eq, vl::EBlendEquation& alpha_eq) const;

    //! The current color mask, see also http://www.opengl.org/sdk/docs/man/xhtml/glColorMask.xml for more information.
    void setColorMask(bool r, bool g, bool b, bool a) { mState.mColorMask = vl::ivec4(r ? 1 : 0, g ? 1 : 0, b ? 1 : 0, a ? 1 : 0); }

    //! The current color mask.
    const vl::ivec4& colorMask() const { return mState.mColorMask; }

    /*void setDetphMask(bool mask) { mState.mDepthMask = mask; }
    bool depthMask() const { return mState.mDepthMask; }*/

    //! If set to 'true' the stencil test and operations will be enabled
    void setStencilTestEnabled(bool enabled) { mState.mStencilTestEnabled = enabled; }

    //! If set to 'true' the stencil test and operations will be enabled
    bool stencilTestEnabled() const { return mState.mStencilTestEnabled; }

    //! Current stencil mask, see also http://www.opengl.org/sdk/docs/man/xhtml/glStencilMask.xml for more information.
    void setStencilMask(unsigned int mask) { mState.mStencilMask = mask; }

    //! Current stencil mask.
    unsigned int stencilMask() const { return mState.mStencilMask; }

    //! Current stencil operation, see also http://www.opengl.org/sdk/docs/man/xhtml/glStencilOp.xml for more information.
    void setStencilOp(vl::EStencilOp sfail, vl::EStencilOp dpfail, vl::EStencilOp dppass);

    //! Current stencil operation.
    void getStencilOp(vl::EStencilOp& sfail, vl::EStencilOp& dpfail, vl::EStencilOp& dppass);

    //! The current stencil function, see also http://www.opengl.org/sdk/docs/man/xhtml/glStencilFunc.xml for more information.
    void setStencilFunc(vl::EFunction func, int refval, unsigned int mask);

    //! The current stencil function.
    void getStencilFunc(vl::EFunction& func, int& refval, unsigned int& mask);

    //! Sets the current Font
    void setFont(const vl::String& name, int size, bool smooth = false) { mState.mFont = vl::defFontManager()->acquireFont(name, size, smooth); }

    //! Sets the current Font
    void setFont(const vl::Font* font) { setFont(font->filePath(), font->size(), font->smooth()); }

    //! Sets the default Font
    void setDefaultFont() { setFont(vl::defFontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 10, false)); }

    //! Returns the current Font
    const vl::Font* font() const { return mState.mFont.get(); }

    /** Defines the scissor box and enables the scissor test.
     * The parameters are considered in windows coordinates.
     * The Scissor is used to clip the rendering against a specific rectangular area.
     * See also http://www.opengl.org/sdk/docs/man/xhtml/glScissor.xml for more information. */
    void setScissor(int x, int y, int width, int height)
    {
        mScissor = resolveScissor(x, y, width, height);
    }

    /** Returns the currently active Scissor */
    const vl::Scissor* scissor() const { return mScissor.get(); }

    /** Disables the Scissor test and clipping. */
    void removeScissor()
    {
        mScissor = NULL;
    }

    /** Clears the specific area of the viewport.
     * The parameters x y w h define a rectangular area in viewport coordinates that is clipped against the viewport itself.
     *
     * \note The specified rectangular area is not affected by the current matrix transform. */
    vl::Actor* clearColor(const vl::fvec4& color, int x = 0, int y = 0, int w = -1, int h = -1);

    /** Clears the specific area of the viewport.
     * The parameters x y w h define a rectangular area in viewport coordinates that is clipped against the viewport itself.
     *
     * \note The specified rectangular area is not affected by the current matrix transform. */
    vl::Actor* clearStencil(int clear_val, int x = 0, int y = 0, int w = -1, int h = -1);

    //! Draw the specified Text object
    vl::Actor* drawText(vl::Text* text);

    /** Draws the specified text at the specified position.
     * Note that the current matrix transform affect the final position, rotation and scaling of the text. */
    vl::Actor* drawText(int x, int y, const vl::String& text, int alignment = vl::AlignBottom | vl::AlignLeft);

    //! Draws the specified text
    vl::Actor* drawText(const vl::String& text, int alignment = vl::AlignBottom | vl::AlignLeft);

    //! 3d版
    vl::Actor* drawText(const vl::fvec3& pos, const vl::String& text, int alignment = vl::AlignBottom | vl::AlignLeft);

    /** Draws the specified Actor with the specified Transform.
     * If keep_effect is set to 'false' or the Actor's Effect is NULL a default Effect is automatically generated.
     * If 'transform' is non NULL it is bound to the Actor. */
    vl::Actor* drawActor(vl::Actor* actor, vl::Transform* transform = NULL, bool keep_effect = false);

    /** Like drawActor() but instead of drawing the given actor creates a copy of it and draws that.
     * This function is useful when you want to crate multiple instances of the same geometry. */
    vl::Actor* drawActorCopy(vl::Actor* actor, vl::Transform* transform = NULL);

    //! Returns the current transform matrix
    const vl::dmat4& matrix() const { return mMatrix; }

    //! Sets the current transform matrix
    void setMatrix(const vl::dmat4& matrix) { mMatrix = matrix; }

    //! Resets the current transform matrix.
    void resetMatrix() { mMatrix.setIdentity(); }

    //! Performs a rotation of 'deg' degrees around the z axis.
    void rotate(double deg);

    //! Translates the current transform matrix
    void translate(double x, double y, double z = 0.0);

    void translate(const vl::dvec3& v);

    //! Scales the current transform matrix
    void scale(double x, double y, double z = 1.0);

    //! Pushes the current matrix in the matrix stack in order to restore it later with popMatrix().
    void pushMatrix() { mMatrixStack.push_back(matrix()); }

    //! Pops the top most matrix in the matrix stack and sets it as the current matrix.
    void popMatrix();

    //! Returns the matrix stack.
    const std::vector<vl::dmat4>& matrixStack() const { return mMatrixStack; }

    //! Pushes the current VectorGraphics state (including the matrix state) in the state stack in order to restore it later with popState().
    void pushState();

    //! Pops the top most state in the state stack and sets it as the current state.
    void popState();

    /*const std::vector<State>& stateStack() const { return mStateStack; }*/

    /** Pushes the current scissor in the scissor stack in order to restore it later with popScissor() and activates a new one.
     * The 'x', 'y', 'w' and 'h' parameters define the new scissor rectangle.
     * Note that such rectangle is clipped against the currently active one. */
    void pushScissor(int x, int y, int w, int h);

    //! Pops the top most scissor in the scissor stack and sets it as the current scissor.
    void popScissor();

    //! Returns the scissor stack.
    const std::vector< vl::ref<vl::Scissor> >& scissorStack() const { return mScissorStack; }

    //! Binds the given Transform to all the Actor[s] that have been generated so far.
    void setTransform(vl::Transform* transform) { for (size_t i = 0; i < actors()->size(); ++i) actors()->at(i)->setTransform(transform); }

    //! Returns the Effect representing the current VectorGraphic's state.
    vl::Effect* currentEffect() { return currentEffect(mState); }


    // render-state设置的辅助函数
    void apply(const KpLine& line);

private:
    void generateQuadsTexCoords(vl::Geometry* geom, int numPoints);

    void generatePlanarTexCoords(vl::Geometry* geom, const std::vector<vl::dvec2>& points);
    void generatePlanarTexCoords(vl::Geometry* geom, const std::vector<vl::dvec3>& points);

    void generateLinearTexCoords(vl::Geometry* geom);

    vl::ref<vl::Geometry> prepareGeometry(const std::vector<vl::dvec2>& ln);
    vl::ref<vl::Geometry> prepareGeometry(const std::vector<vl::dvec3>& ln);

    vl::ref<vl::Geometry> prepareGeometryPolyToTriangles(const std::vector<vl::dvec2>& ln);
    vl::ref<vl::Geometry> prepareGeometryPolyToTriangles(const std::vector<vl::dvec3>& ln);

    vl::Scissor* resolveScissor(int x, int y, int width, int height);

    vl::Texture* resolveTexture(const vl::Image* image);

    vl::Effect* currentEffect(const State& vgs);

    vl::Actor* addActor(vl::Actor* actor);

    vl::Actor* addActor_(vl::Geometry* geom);

    vl::Actor* drawLines_(vl::Geometry* geom, int numPoints);

    vl::Actor* drawLineStrip_(vl::Geometry* geom, int numPoints);

    vl::Actor* drawLineLoop_(vl::Geometry* geom, int numPoints);

    template<typename T>
    vl::Actor* fillTriangles_(const std::vector<T>& triangles, vl::EPrimitiveType type) {
        // fill the vertex position array
        vl::ref<vl::Geometry> geom = prepareGeometry(triangles);
        // generate texture coords
        generatePlanarTexCoords(geom.get(), triangles);
        // issue the primitive
        geom->drawCalls().push_back(new vl::DrawArrays(type, 0, (int)triangles.size()));
        // add the actor
        return addActor_(geom.get());
    }

private:
    // state-machine state variables
    State mState;
    vl::dmat4 mMatrix;
    vl::ref<vl::Scissor> mScissor;
    std::vector<State> mStateStack;
    std::vector<vl::dmat4> mMatrixStack;
    std::vector< vl::ref<vl::Scissor> > mScissorStack;
    // state-machine state map
    std::map<State, vl::ref<vl::Effect> > mVGToEffectMap;
    std::map<ImageState, vl::ref<vl::Texture> > mImageToTextureMap;
    std::map<vl::RectI, vl::ref<vl::Scissor> > mRectToScissorMap;
    vl::ref<vl::Effect> mDefaultEffect;
    vl::ActorCollection mActors;
};
