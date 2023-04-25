#include "KcGlslProgram.h"
#include "glad.h"
#include "KcGlslShader.h"
#include <assert.h>


KcGlslProgram::KcGlslProgram()
{
	scheduleLink_ = true;
	handle_ = 0;
	programBinaryRetrievableHint_ = false;
	programSeparable_ = false;

	resetBindingLocations_();
}


KcGlslProgram::~KcGlslProgram()
{
	if (handle())
	    destroy();
}


void KcGlslProgram::resetBindingLocations_()
{
	// standard uniform binding
	modelViewMatrix_ = -1;
	projectionMatrix_ = -1;
	modelViewProjectionMatrix_ = -1;
	normalMatrix_ = -1;

	// vertex attrib binding
	vertexPosition_ = -1;
	vertexNormal_ = -1;
	vertexColor_ = -1;
	vertexSecondaryColor_ = -1;
	vertexFogCoord_ = -1;
	vertexTexCoord0_ = -1;
	vertexTexCoord1_ = -1;
	vertexTexCoord2_ = -1;
	vertexTexCoord3_ = -1;
	vertexTexCoord4_ = -1;
	vertexTexCoord5_ = -1;
	vertexTexCoord6_ = -1;
	vertexTexCoord7_ = -1;
	vertexTexCoord8_ = -1;
	vertexTexCoord9_ = -1;
	vertexTexCoord10_ = -1;
}


bool KcGlslProgram::reload()
{
	return true;
}


void KcGlslProgram::create()
{
	if (handle() == 0) {
		scheduleRelinking_();
		handle_ = glCreateProgram();
		assert(handle() && glIsProgram(handle()));
	}
}


void KcGlslProgram::destroy()
{
	if (handle()) {
		glDeleteProgram(handle()); 
		handle_ = 0;
	}
	resetBindingLocations_();
	scheduleRelinking_();
}


bool KcGlslProgram::link(bool forceRelink)
{
	if (linked() && !forceRelink) 
		return true;

	resetBindingLocations_();

	if (shaderCount() == 0)
		return false; // no shaders

	create();

	// pre-link operations
	preLink_();

	// link the program

	glLinkProgram(handle());

	scheduleLink_ = !linkStatus();

	// check link error
	if (!linked())
		return false;

	// post-link operations
	postLink_();

	return true;
}


void KcGlslProgram::preLink_()
{
	
}


void KcGlslProgram::postLink_()
{

}


bool KcGlslProgram::linkStatus() const
{
	if (handle() == 0)
		return false;

	int status = 0;
	glGetProgramiv(handle(), GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}


std::string KcGlslProgram::infoLog() const
{
	if (handle() == 0) 
		return "GLSL program object not yet created!";

	int max_length = 0;
	glGetProgramiv(handle(), GL_INFO_LOG_LENGTH, &max_length);
	std::vector<char> log_buffer;
	log_buffer.resize(max_length + 1);
	glGetProgramInfoLog(handle(), max_length, NULL, log_buffer.data()); 
	return log_buffer.data();
}


bool KcGlslProgram::validate() const
{
	if (handle() == 0)
		return false;

	GLint status = 0;
	glValidateProgram(handle());
	glGetProgramiv(handle(), GL_VALIDATE_STATUS, &status); 
	return status == GL_TRUE;
}


void KcGlslProgram::bindAttribLocation(unsigned int index, const std::string_view& name)
{
	create();
	scheduleRelinking_();
	glBindAttribLocation(handle(), index, name.data());
}


int KcGlslProgram::getAttribLocation(const std::string_view& name) const
{
	return glGetAttribLocation(handle(), name.data());
}


bool KcGlslProgram::attachShader(std::shared_ptr<KcGlslShader> shader)
{
	create();

	scheduleRelinking_();

	detachShader(shader.get());
	shaders_.emplace_back(shader);

	glAttachShader(handle(), shader->handle());

	return shader->compileStatus();
}


bool KcGlslProgram::detachShader(KcGlslShader* shader)
{
	// if it fails the shader has never been attached to any GLSL program
	for (auto i = shaders_.cbegin(); i != shaders_.cend(); ++i) {
		if (i->get() == shader) {
			if (shader->handle())
				glDetachShader(handle(), shader->handle()); 
			shaders_.erase(i);
			return true;
		}
	}

	return false;
}


void KcGlslProgram::detachAllShaders(bool deleteShaders)
{
	for (auto i = shaders_.cbegin(); i != shaders_.cend(); ++i)
		if ((*i)->handle()) {
			glDetachShader(handle(), (*i)->handle());
			if (deleteShaders) (*i)->destroy();
		}

	shaders_.clear();
	scheduleRelinking_();
}


int KcGlslProgram::getUniformLocation(const std::string_view& name) const
{
	return glGetUniformLocation(handle(), name.data());
}


void KcGlslProgram::getUniformfv(int location, float* params) const
{
	glGetUniformfv(handle(), location, params);
}


void KcGlslProgram::getUniformiv(int location, int* params) const
{
	glGetUniformiv(handle(), location, params); 
}


void KcGlslProgram::useProgram()
{
	if (!linked())
		link(true);

	useProgram(handle());
}


unsigned KcGlslProgram::currentProgram()
{
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	return static_cast<unsigned>(prog);
}


void KcGlslProgram::useProgram(unsigned prog)
{
	glUseProgram(prog);
}


void KcGlslProgram::setUniform(const std::string_view& name, const mat3f<>& v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1) {
		if constexpr (std::remove_reference_t<decltype(v)>::rowMajor())
			glUniformMatrix3fv(loc, 1, GL_TRUE, v.data());
		else
			glUniformMatrix3fv(loc, 1, GL_FALSE, v.data());
	}
}


void KcGlslProgram::setUniform(const std::string_view& name, const mat4f<>& v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1) {
		if constexpr (std::remove_reference_t<decltype(v)>::rowMajor())
			glUniformMatrix4fv(loc, 1, GL_TRUE, v.data());
		else
			glUniformMatrix4fv(loc, 1, GL_FALSE, v.data());
	}
}


void KcGlslProgram::setUniform(const std::string_view& name, float v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1)
		glUniform1f(loc, v);
}


void KcGlslProgram::setUniform(const std::string_view& name, const point2f& v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1)
		glUniform2f(loc, v[0], v[1]);
}


void KcGlslProgram::setUniform(const std::string_view& name, const point3f& v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1)
		glUniform3f(loc, v[0], v[1], v[2]);
}


void KcGlslProgram::setUniform(const std::string_view& name, const point4f& v)
{
	auto loc = getUniformLocation(name);
	if (loc != -1)
		glUniform4f(loc, v[0], v[1], v[2], v[3]);
}

