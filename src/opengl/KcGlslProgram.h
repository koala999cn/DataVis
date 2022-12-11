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
    void createProgram();

    // Deletes the GLSL program calling glDeleteProgram(handle())
    // After this function handle() will return 0.
    void deleteProgram();

    // The handle of the GLSL program as returned by glCreateProgram()
    unsigned int handle() const { return handle_; }


private:
    void resetBindingLocations_();

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