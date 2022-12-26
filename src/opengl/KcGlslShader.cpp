#include "KcGlslShader.h"
#include "KuPathUtil.h"
#include "KuFileUtil.h"
#include "glad.h"


KcGlslShader::KcGlslShader(KeType type, const std::string_view& path_or_source)
    : type_(type)
{
	if (KuPathUtil::isLegalFileName(path_or_source.data()) && KuPathUtil::exist(path_or_source)) {
        path_ = path_or_source;
        source_ = KuFileUtil::readAsString(path_);
	}
    else {
        source_ = path_or_source;
    }

    compile();
}


KcGlslShader::~KcGlslShader()
{
    destroy();
}


void KcGlslShader::create_()
{
    const static GLenum mapShaderType[] = {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_GEOMETRY_SHADER,
        GL_TESS_CONTROL_SHADER,
        GL_TESS_EVALUATION_SHADER
    };

    if (!handle()) 
        handle_ = glCreateShader(mapShaderType[type_]);

    assert(glIsShader(handle())); // TODO:
}


void KcGlslShader::destroy()
{
    if (handle()) {
        glDeleteShader(handle()); 
        handle_ = 0;
    }
}


std::string KcGlslShader::getShaderSource() const
{
    if (handle()) {
        GLint len = 0;
        glGetShaderiv(handle(), GL_SHADER_SOURCE_LENGTH, &len);
        if (len) {
            std::vector<char> src;
            src.resize(len);
            GLint len_written = 0;
            glGetShaderSource(handle(), len, &len_written, &src[0]);
            return src.data();
        }
    }

    return "";
}


bool KcGlslShader::compile()
{
    if (compileStatus())
        return true;

    // make sure shader object exists
    create_();

    // assign sources
    const char* source[] = { source_.c_str() };
    glShaderSource(handle(), 1, source, NULL);

    // compile the shader
    glCompileShader(handle());

    return compileStatus();
}


bool KcGlslShader::compileStatus() const
{
    if (!handle())
        return false;

    int status = 0;
    glGetShaderiv(handle(), GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}


bool KcGlslShader::reload()
{
    if (!path_.empty()) {
        source_ = KuFileUtil::readAsString(path_);
        return compile();
    }

    return false;
}


std::string KcGlslShader::infoLog() const
{
    assert(handle());

    int max_length = 0;
    glGetShaderiv(handle(), GL_INFO_LOG_LENGTH, &max_length); 
    if (max_length == 0)
        return "";
    
    std::vector<char> log_buffer;
    log_buffer.resize(max_length);
    glGetShaderInfoLog(handle(), max_length, NULL, log_buffer.data());
    return log_buffer.data();
}
