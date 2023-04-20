#ifndef EXAMPLEAPP_H
#define EXAMPLEAPP_H

#include <api/MinVR.h>
using namespace MinVR;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "TurntableManipulator.h"

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include "cyHairFile.h"
#include <BasicGraphics.h>
using namespace basicgraphics;

class HairSimulation : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
	HairSimulation(int argc, char** argv);
    virtual ~HairSimulation();

    
    /** USER INTERFACE CALLBACKS **/
    virtual void onAnalogChange(const VRAnalogEvent &state);
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
	virtual void onCursorMove(const VRCursorEvent &state);
    virtual void onTrackerMove(const VRTrackerEvent &state);
    
    
    /** RENDERING CALLBACKS **/
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    
    
private:
    
    GLuint _vaoID;
    GLuint _vertexVBO;
    cyHairFile hair;
    float *dirs;
    
    void LoadHairModel( const char *filename, cyHairFile &hairfile, float *&dirs );
    void DrawHairModel( const cyHairFile &hairfile, float *dirs );
    
    std::shared_ptr<TurntableManipulator> _turntable;

    bool mouseDown;
    
    glm::vec4 _lightPosition;
    bool _drawLightVector;
    
    std::shared_ptr<Texture> _lookUp1;
    std::shared_ptr<Texture> _lookUp2;

	virtual void reloadShaders();
	GLSLProgram _shader;
};

#endif //EXAMPLEAPP_H
