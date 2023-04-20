#include "HairSimulation.h"
#include <string.h>
#include "cyHairFile.h"
using namespace basicgraphics;
using namespace glm;

#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

void HairSimulation::reloadShaders()
{
    _shader.compileShader("hair.vert", GLSLShader::VERTEX);
    _shader.compileShader("marschnerHair.frag", GLSLShader::FRAGMENT);
    _shader.link();
    _shader.use();
}

void HairSimulation::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame.
    
	
    if (renderState.isInitialRenderCall()) {

		//For windows, we need to initialize a few more things for it to recognize all of the
		// opengl calls.
		#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
		#endif     


        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();
        
        _lookUp1 = Texture::create2DTextureFromFile("lookup1.png");
        _lookUp1->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        _lookUp1->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        _lookUp2 = Texture::create2DTextureFromFile("lookup2.png");
        _lookUp2->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        _lookUp2->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        LoadHairModel("dark.hair", hair, dirs);
    }
}

void HairSimulation::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
	glm::vec3 eye_world = _turntable->getPos();
	glm::mat4 view = _turntable->frame();
	// When we use virtual reality, this will be replaced by:
	// eye_world = glm::make_vec3(renderState.getCameraPos())
	// view = glm::make_mat4(renderState.getViewMatrix());

	// Setup the projection matrix so that things are rendered in perspective
	GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
	GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 500.0f);
	// When we use virtual reality, this will be replaced by:
	// projection = glm::make_mat4(renderState.getProjectionMatrix())
	
	// Setup the model matrix
	glm::mat4 model = glm::mat4(1.0);
    
    glm::mat4 lightView = lookAt(vec3(_lightPosition), vec3(0,0,0), vec3(0,1,0));
    
	// Tell opengl we want to use this specific shader.
	_shader.use();
	
	_shader.setUniform("view_mat", view);
	_shader.setUniform("projection_mat", projection);
	
    _shader.setUniform("lightView", lightView);
	_shader.setUniform("model_mat", model);
	_shader.setUniform("tangent_mat", mat3(model));
	_shader.setUniform("eye_world", eye_world);
    
    //Cook-Torrance constants
    vec3 ambientReflectionCoeff = vec3(0.4125, 0.275, 0.0375);
    vec3 diffuseReflectionCoeff = vec3(0.78, 0.57, 0.11);
    vec3 specularReflectionCoeff = vec3(0.99, 0.94, 0.80);
    
    vec3 ambientLightIntensity = vec3(0.4, 0.4, 0.4);
    vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
    vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);
    
    float m = 0.55;
    float r0 = 0.7;
    
    _shader.setUniform("ambientReflectionCoeff", ambientReflectionCoeff);
    _shader.setUniform("diffuseReflectionCoeff", diffuseReflectionCoeff);
    _shader.setUniform("specularReflectionCoeff", specularReflectionCoeff);
    
    
    _shader.setUniform("ambientLightIntensity", ambientLightIntensity);
    _shader.setUniform("diffuseLightIntensity", diffuseLightIntensity);
    _shader.setUniform("specularLightIntensity", specularLightIntensity);
    
    _shader.setUniform("r0", r0);
    _shader.setUniform("m", m);
    
    _shader.setUniform("lightPosition", _lightPosition);
    
     DrawHairModel(hair, dirs);
    
    // For debugging purposes, let's draw a sphere to reprsent each "light bulb" in the scene, that way
    // we can make sure the lighting on the bunny makes sense given the position of each light source.
    Sphere s(vec3(_lightPosition), 5.0f, vec4(1.0f, 1.0f, 0.0f, 1.0f));
    s.draw(_shader, glm::mat4(1.0));
    
    // Another useful aid for debugging: draw vectors to the light sources
    if (_drawLightVector) {
        Cylinder l(vec3(0, -5, 10), vec3(_lightPosition), 0.3f, vec4(1.0f, 1.0f, 0.0f, 1.0f));
        l.draw(_shader, glm::mat4(1.0));
    }
}

void HairSimulation::LoadHairModel( const char *filename, cyHairFile &hairfile, float *&dirs )
{
    // Load the hair model
    int result = hairfile.LoadFromFile( filename );
    // Check for errors
    switch( result ) {
        case CY_HAIR_FILE_ERROR_CANT_OPEN_FILE:
            printf("Error: Cannot open hair file!\n");
            return;
        case CY_HAIR_FILE_ERROR_CANT_READ_HEADER:
            printf("Error: Cannot read hair file header!\n");
            return;
        case CY_HAIR_FILE_ERROR_WRONG_SIGNATURE:
            printf("Error: File has wrong signature!\n");
            return;
        case CY_HAIR_FILE_ERROR_READING_SEGMENTS:
            printf("Error: Cannot read hair segments!\n");
            return;
        case CY_HAIR_FILE_ERROR_READING_POINTS:
            printf("Error: Cannot read hair points!\n");
            return;
        case CY_HAIR_FILE_ERROR_READING_COLORS:
            printf("Error: Cannot read hair colors!\n");
            return;
        case CY_HAIR_FILE_ERROR_READING_THICKNESS:
            printf("Error: Cannot read hair thickness!\n");
            return;
        case CY_HAIR_FILE_ERROR_READING_TRANSPARENCY:
            printf("Error: Cannot read hair transparency!\n");
            return;
        default:
            printf("Hair file \"%s\" loaded.\n", filename);
    }
    int hairCount = hairfile.GetHeader().hair_count;
    int pointCount = hairfile.GetHeader().point_count;
    printf("Number of hair strands = %d\n", hairCount );
    printf("Number of hair points = %d\n", pointCount );
    
    dirs = new float[3 * pointCount];
    // Compute directions
    if( hairfile.FillDirectionArray( dirs ) == 0 ) {
        printf("Error: Cannot compute hair directions!\n");
    }
    
    // create the vao
    glGenVertexArrays(1, &_vaoID);
    glBindVertexArray(_vaoID);
    
    // create the vbo
    glGenBuffers(1, &_vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO);
    
    int allocateVertexByteSize = 3*(3*sizeof(float)*pointCount);
    
    // initialize size
    glBufferData(GL_ARRAY_BUFFER, allocateVertexByteSize, NULL, GL_STATIC_DRAW);
    
    int dataByteSize = 3*sizeof(float)*pointCount;
    //buffer data
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataByteSize, hairfile.GetPointsArray());
    glBufferSubData(GL_ARRAY_BUFFER, dataByteSize, dataByteSize, dirs);
    glBufferSubData(GL_ARRAY_BUFFER, 2*dataByteSize, dataByteSize, hairfile.GetColorsArray());
    
    // set up vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(dataByteSize));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(2*dataByteSize));
    
    // unbind the vao
    glBindVertexArray(0);
    
    vec3 centroid;
    
    const float* points = hairfile.GetPointsArray();
    for ( int i=0; i < pointCount; i+=3 ) {
        vec3 point = vec3 (points[i], points[i+1], points[i+2]);
        centroid += point;
    }
    centroid /= pointCount;
    cout<<"Centroid: "<< centroid.x<<" "<<centroid.y<<" "<<centroid.z<<endl;
}

void HairSimulation::DrawHairModel( const cyHairFile &hairfile, float *dirs )
{
    glBindVertexArray(_vaoID);
    
    // Draw arrays
    int pointIndex = 0;
    int hairCount = hairfile.GetHeader().hair_count;
    const unsigned short *segments = hairfile.GetSegmentsArray();
    if ( segments ) {
        // If segments array exists
        for ( int hairIndex=0; hairIndex < hairCount; hairIndex++ ) {
            glDrawArrays( GL_LINE_STRIP, pointIndex, segments[ hairIndex ]+1 );
            pointIndex += segments[ hairIndex ]+1;
        }
    } else {
        // If segments array does not exist, use default segment count
        int dsegs = hairfile.GetHeader().d_segments;
        for ( int hairIndex=0; hairIndex < hairCount; hairIndex++ ) {
            glDrawArrays( GL_LINE_STRIP, pointIndex, dsegs+1 );
            pointIndex += dsegs+1;
        }
    }
    
    glBindVertexArray(0);
