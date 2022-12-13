#pragma once
#include <string_view>


// 实现OpenGL的shader封装
// 初始版本参考Visualization Library

class KcGlslShader
{
public:

	enum KeType
	{
		k_shader_vertex,
		k_shader_fragment
	};

	KcGlslShader(KeType type, const std::string_view& path_or_source);

	~KcGlslShader();

	// Reloads the shader source and recompiles it. Returns true on success.
	bool reload();

	// The handle of this OpenGL shader object as returned by glCreateShader()
	unsigned int handle() const { return handle_; }

	// Retrieves the shader source using glGetShaderSource()
	std::string getShaderSource() const;

	// Compiles the shader
	// This function also create the shader if handle() == 0 using the OpenGL function glCreateShader()
	bool compile();

	// Returns true if the shader has been succesfully compiled.
	// The check is done using the OpenGL function glGetShaderiv()
	bool compileStatus() const;

	// Returns a String object containing this shader's info log as returned by glGetShaderInfoLog(), see also http://www.opengl.org/sdk/docs/man/xhtml/glGetShaderInfoLog.xml for more information.
	std::string infoLog() const;

	// Deletes the shader using the OpenGL function glDeleteShader()
	void destroy();

private:

	// Creates the shader using the OpenGL function glCreateShader()
	void create_();

private:
	KeType type_;
	std::string path_;
	std::string source_;
	unsigned int handle_{ 0 };
};
