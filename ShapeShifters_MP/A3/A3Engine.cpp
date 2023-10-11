#include "A3Engine.h"

#include <CSCI441/objects.hpp>
#include <iostream>
#include <ctime>
//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265f
#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRandFloat() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}
GLint getRand() {
    return (GLint)rand() % 8;   // changed function to get random int between 0 and 7
}

//*************************************************************************************
//
// Public Interface

A3Engine::A3Engine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "Shape Shifters: Graphics MP") {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    _arcBallCamSpeed = 0.05f;
    _isSkipShowers = true; _isBith = false; _isArcballCam = true; _isFirstPersonCam = false; _isFreeCam = false;
}

A3Engine::~A3Engine() {
    delete _pSkipShowers;
    delete _pBith;
    delete _pFreeCam;
}

void A3Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void A3Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void A3Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        if (_isArcballCam) {
            if(_isSkipShowers) {
                _pSkipShowers->getArcballCam()->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                                       (currMousePosition.y - _mousePosition.y) * 0.005f);
            }
            else if(_pBith){
                _pBith->getArcballCam()->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                                       (currMousePosition.y - _mousePosition.y) * 0.005f);
            }
        }
        else if (_isFreeCam) {
            _pFreeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                (_mousePosition.y - currMousePosition.y) * 0.005f);
        }
    }


    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void A3Engine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, lab05_engine_cursor_callback);
}

void A3Engine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                        // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void A3Engine::mSetupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/lab05.v.glsl", "shaders/lab05.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");
    // assign uniforms
    _lightingShaderUniformLocations.normalMtx = _lightingShaderProgram->getUniformLocation("normalMtx");
    _lightingShaderUniformLocations.lightDirection = _lightingShaderProgram->getUniformLocation("lightDirection");
    _lightingShaderUniformLocations.lightColor = _lightingShaderProgram->getUniformLocation("lightColor");

    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    // assign attributes
    _lightingShaderAttributeLocations.vNormal = _lightingShaderProgram->getAttributeLocation("vNormal");

}

void A3Engine::mSetupBuffers() {
    // need to connect our 3D Object Library to our shader
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNormal);
    // shaders implementation
    _pSkipShowers = new SkipShowers(_lightingShaderProgram->getShaderProgramHandle(),
                                    _lightingShaderUniformLocations.mvpMatrix,
                                    _lightingShaderUniformLocations.normalMtx,
                                    _lightingShaderUniformLocations.materialColor);

    _pBith = new Vehicle(_lightingShaderProgram->getShaderProgramHandle(),
                                             _lightingShaderUniformLocations.mvpMatrix,
                                             _lightingShaderUniformLocations.normalMtx,
                                             _lightingShaderUniformLocations.materialColor);

    _createGroundBuffers();
    _generateEnvironment();
}

void A3Engine::_createGroundBuffers() {
    // expand our struct
    struct Vertex {
        GLfloat x, y, z, normal_x, normal_y, normal_z;
    };

    // add normal data
    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            {-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f}
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    // hook up vertex normal attribute
    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vNormal);
    GLuint normalOffset = sizeof(GLfloat) * 3;  // used in place of nullptr in the previous glVertexAttribPointer
    glVertexAttribPointer(_lightingShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)normalOffset);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void A3Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, you can
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************
    // use a designated color palette to make the city feel more realistic
    buildingColors[0] = glm::vec3(1.0f, 1.0f, 1.0f);    // white
    buildingColors[1] = glm::vec3(0.1f, 0.1f, 0.1f);    // black-ish
    buildingColors[2] = glm::vec3(0.25f, 0.25f, 0.25f);    // dark grey
    buildingColors[3] = glm::vec3(0.5f, 0.5f, 0.5f);    // grey
    buildingColors[4] = glm::vec3(0.75f, 0.75f, 0.75f);    // light grey
    buildingColors[5] = glm::vec3(0.898f, 0.827f, 0.702f);    // tan
    buildingColors[6] = glm::vec3(0.725f, 0.600f, 0.463f);    // brown
    buildingColors[7] = glm::vec3(0.624f, 0.867f, 0.910f);    // light blue

    srand( time(0) );                                                   // seed our RNG

    // psych! everything's on a grid.
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {
            // don't just draw a building ANYWHERE.
            if( i % 2 && j % 2 && getRandFloat() < 0.4f ) {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate( glm::mat4(1.0), glm::vec3(i, 0.0f, j) );

                // compute random height
                GLdouble height = powf(getRandFloat(), 2.5)*10 + 1;
                // scale to building size
                glm::mat4 scaleToHeightMtx = glm::scale( glm::mat4(1.0), glm::vec3(1, height, 1) );

                // translate up to grid
                glm::mat4 transToHeight = glm::translate( glm::mat4(1.0), glm::vec3(0, height/2.0f, 0) );

                // compute full model matrix
                glm::mat4 modelMatrix = transToHeight * scaleToHeightMtx * transToSpotMtx;

                // compute random color
                glm::vec3 color( buildingColors[getRand()]);
                // store building properties
                BuildingData currentBuilding = {modelMatrix, color};
                _buildings.emplace_back( currentBuilding );
            }
        }
    }
}

void A3Engine::mSetupScene() {
    // setup free cam (press 2)
    _pFreeCam = new CSCI441::FreeCam();
    _pFreeCam->setPosition(glm::vec3(60.0f, 40.0f, 30.0f));
    _pFreeCam->setTheta(-M_PI / 3.0f);
    _pFreeCam->setPhi(M_PI / 2.8f);
    _pFreeCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);
    // set lighting uniforms
    glm::vec3 lightDirection = glm::vec3(-1,-1,-1);
    glm::vec3 lightColor = glm::vec3(1,1,1);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirection, 1, &lightDirection[0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor, 1, &lightColor[0]);
}

//*************************************************************************************
//
// Engine Cleanup

void A3Engine::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}

void A3Engine::mCleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _pSkipShowers;
    delete _pBith;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void A3Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();


    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.7f, 0.7f, 0.7f);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, groundColor);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUILDINGS ////
    for( const BuildingData& currentBuilding : _buildings ) {
        _computeAndSendMatrixUniforms(currentBuilding.modelMatrix, viewMtx, projMtx);

        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, currentBuilding.color);

        CSCI441::drawSolidCube(1.0);
    }
    //// END DRAWING THE BUILDINGS ////

    //// BEGIN DRAWING HERO ////
    glm::mat4 modelMtx(1.0f);
    // the y value in glm::vec3 is what sets the hero at the correct height to not pass through the ground
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.32f, 0.0f));
    // rotate the hero with our camera theta direction (we need to rotate the opposite direction so that we always look at the back)
    modelMtx = glm::rotate(modelMtx, 0.0f, CSCI441::Y_AXIS );
    // rotate the hero with our camera phi direction
    modelMtx = glm::rotate(modelMtx, 1.57f, CSCI441::X_AXIS );
    // draw hero now

        _pSkipShowers->drawHero(modelMtx, viewMtx, projMtx);
        _pBith->drawWagon(modelMtx, viewMtx, projMtx);
    //// END DRAWING HERO ////
}

void A3Engine::_updateScene() {
    // move
    if( _keys[GLFW_KEY_SPACE] ) {
        // zoom out if shift held down
        if( (_keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT]) &&  _isArcballCam ) {
            if(_isSkipShowers) {
                _pSkipShowers->getArcballCam()->moveBackward(_arcBallCamSpeed);
            }
            else if(_isBith){
                _pBith->getArcballCam()->moveBackward(_arcBallCamSpeed);
            }
        }
        else if ((_keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT]) && _isFreeCam) {
            _pFreeCam->moveBackward(_cameraSpeed.x);
        }
        // zoom in if space pressed and no shift
        else if (_isArcballCam) {
            if(_isSkipShowers) {
                _pSkipShowers->getArcballCam()->moveForward(_arcBallCamSpeed);
            }
            else if(_isBith){
                _pBith->getArcballCam()->moveForward(_arcBallCamSpeed);
            }
        }
        else if (_isFreeCam) {
            _pFreeCam->moveForward(_cameraSpeed.x);
        }
    }
    // turn right
    if( _keys[GLFW_KEY_D] || _keys[GLFW_KEY_RIGHT] ) {
        if(_isSkipShowers){
            _pSkipShowers->turnRight();
        }
        else if(_isBith){
            _pBith->turnRight();
        }
    }
    // turn left
    if( _keys[GLFW_KEY_A] || _keys[GLFW_KEY_LEFT] ) {
        if(_isSkipShowers) {
            _pSkipShowers->turnLeft();
        }
        else if(_isBith){
            _pBith->turnLeft();
        }
    }
    // move forward
    if( _keys[GLFW_KEY_W] || _keys[GLFW_KEY_UP] ) {
        if(_isSkipShowers) {
            _pSkipShowers->moveForwards();
        }
        else if(_isBith) {
            _pBith->moveForwards();
        }
    }
    // move backward
    if( _keys[GLFW_KEY_S] || _keys[GLFW_KEY_DOWN] ) {
        if(_isSkipShowers) {

            _pSkipShowers->moveBackwards();
        }
        else if(_isBith){
            _pBith->moveBackwards();
        }
    }

    // change camera view
    if (_keys[GLFW_KEY_1]) {   // set camera to ArcballCam
        _isArcballCam = true;
        _isFreeCam = false;
        _isFirstPersonCam = false;


    }
    else if (_keys[GLFW_KEY_2]) {   // set camera to FreeCam
        _isFreeCam = true;
        _isArcballCam = false;
        _isFirstPersonCam = false;

    }
    else if (_keys[GLFW_KEY_3]) {  // set camera to first person
        _isFirstPersonCam = true;
        _isArcballCam = false;
        _isFreeCam = false;


    }
    else if(_keys[GLFW_KEY_0]){

        if(_isSkipShowers){
            _isSkipShowers = false;
            _isBith = true;
        }
        else if(_isBith){
            _isSkipShowers = true;
            _isBith = false;
        }

    }
}

void A3Engine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(mpWindow) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // draw everything to the window (based on current camera view)
        if (_isArcballCam) {
            if(_isSkipShowers) {
                _renderScene(_pSkipShowers->getArcballCam()->getViewMatrix(),
                             _pSkipShowers->getArcballCam()->getProjectionMatrix());
            }
            else if(_isBith){
                _renderScene(_pBith->getArcballCam()->getViewMatrix(),
                             _pBith->getArcballCam()->getProjectionMatrix());
            }
        }
        else if (_isFreeCam) {
            _renderScene(_pFreeCam->getViewMatrix(), _pFreeCam->getProjectionMatrix());
        }
        else if (_isFirstPersonCam) {
            if(_isSkipShowers) {


                _renderScene(_pSkipShowers->getFirstPersonCam()->getViewMatrix(),
                             _pSkipShowers->getFirstPersonCam()->getProjectionMatrix());
            }
            else if(_isBith){
                _renderScene(_pBith->getFirstPersonCam()->getViewMatrix(),
                             _pBith->getFirstPersonCam()->getProjectionMatrix());
            }
        }
        _updateScene();

        glfwSwapBuffers(mpWindow);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper Functions

void A3Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    // compute and send the normal matrix
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMtx, normalMtx);
}

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
