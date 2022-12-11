#include "KcGlslProgram.h"


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
	    deleteProgram();
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


void KcGlslProgram::createProgram()
{

}


void KcGlslProgram::deleteProgram()
{

}