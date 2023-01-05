#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>

class KcGlslShader;

// OpenGL的Program封装，用于绑定Shader
// 初始版本参考Visualization Library

class KcGlslProgram
{
public:

    // Constructor.
    KcGlslProgram();

    // Destructor. Calls deleteProgram().
    ~KcGlslProgram();


    // Reloads all the shaders source and recompiles them and relinks. Returns true on success.
    bool reload();

    // Calls glCreateProgram() in order to acquire a GLSL program handle
    // The program is created only if handle() == 0
    void create();

    // Deletes the GLSL program calling glDeleteProgram(handle())
    // After this function handle() will return 0.
    void destroy();

    // The handle of the GLSL program as returned by glCreateProgram()
    unsigned int handle() const { return handle_; }

    // Links the GLSLProgram calling glLinkProgram(handle()) only if the program needs to be linked.
    bool link(bool forceRelink = false);

    bool linkStatus() const;

    // Returns true if the program has been succesfully linked.
    bool linked() const { return handle_ && !scheduleLink_; }

    // Returns the info log of this GLSL program using the OpenGL function glGetProgramInfoLog()
    std::string infoLog() const;

    // Returns true if the validation of this GLSL program is succesful
    bool validate() const;

    /** Equivalent to glBindAttribLocation(handle(), index, name.c_str()) with the difference that this function will automatically create a GLSL program if none is present
      * and it will schedule a re-link since the new specified bindings take effect after linking the GLSL program. */
    void bindAttribLocation(unsigned int index, const std::string_view& name);

    // Eqivalento to glGetAttribLocation(handle(), name).
    // The program must be linked before calling this function.
    int getAttribLocation(const std::string_view& name) const;


    // --------------- shaders ---------------

    /**
     * Attaches the GLSLShader to this GLSLProgram
     * Attaching a shader triggers the compilation of the shader (if not already compiled) and relinking of the program.
    */
    bool attachShader(std::shared_ptr<KcGlslShader> shader);

    // Detaches a GLSLShader from the GLSLShader (note: it does NOT schedule a relink of the program)
    bool detachShader(KcGlslShader* shader);

    // Detaches all the shaders and deletes them according to deleteShaders (note that the GLSL Program remains still valid).
    // Use this function when your GLSL program compiled well, you don't want to re-link or re-compile it and you want to save
    // some memory by discarding unnecessary shaders objects.
    // this method will schedules a relinking
    void detachAllShaders(bool deleteShaders);

    // Returns the number of KcGlslShader objects bound to this GLSLProgram
    unsigned shaderCount() const { return shaders_.size(); }

    // Returns the i-th KcGlslShader objects bound to this GLSLProgram
    const KcGlslShader* shaderAt(unsigned i) const { return shaders_[i].get(); }

    //! Returns the i-th GLSLShader objects bound to this GLSLProgram
    KcGlslShader* shaderAt(unsigned i) { return shaders_[i].get(); }


    // --------------- uniform variables ---------------

    /**
    * Returns the binding index of the given uniform.
    */
    int getUniformLocation(const std::string_view& name) const;

    // general uniform getters: use these to access to all the types supported by your GLSL implementation,
    // and not only the ordinary fvec2, fvec3, fvec4, ivec2, ivec3, ivec4, fmat2, fmat3, fmat4

    // Equivalent to glGetUniformfv(handle(), location, params)
    void getUniformfv(int location, float* params) const;

    void getUniformfv(const std::string_view& name, float* params) const { 
        getUniformfv(getUniformLocation(name), params); 
    }

    // Equivalent to glGetUniformiv(handle(), location, params)
    void getUniformiv(int location, int* params) const;

    // Equivalent to getUniformiv(getUniformLocation(name)
    void getUniformiv(const char* name, int* params) const { 
        getUniformiv(getUniformLocation(name), params); 
    }

    void useProgram();

    static unsigned currentProgram();

    static void useProgram(unsigned prog);

private:
    void resetBindingLocations_();

    //! Schedules a relink of the GLSL program.
    void scheduleRelinking_() { scheduleLink_ = true; }

    void preLink_();
    void postLink_();

private:

    std::vector<std::shared_ptr<KcGlslShader>> shaders_;
    std::map<std::string, int> fragDataLocation_;
    unsigned int handle_;
    bool scheduleLink_;

    // glProgramParameter
    bool programBinaryRetrievableHint_;
    bool programSeparable_;

    // VL standard uniforms

    int worldMatrix_;
    int modelViewMatrix_;
    int projectionMatrix_;
    int modelViewProjectionMatrix_;
    int normalMatrix_;

    // VL standard vertex attributes

    int vertexPosition_;
    int vertexNormal_;
    int vertexColor_;
    int vertexSecondaryColor_;
    int vertexFogCoord_;
    int vertexTexCoord0_;
    int vertexTexCoord1_;
    int vertexTexCoord2_;
    int vertexTexCoord3_;
    int vertexTexCoord4_;
    int vertexTexCoord5_;
    int vertexTexCoord6_;
    int vertexTexCoord7_;
    int vertexTexCoord8_;
    int vertexTexCoord9_;
    int vertexTexCoord10_;
};