#include "Vehicle.h"
#include "ArcBallCam.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>
#include <iostream>


void walkHero();

Vehicle::Vehicle(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = _PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = _PI / 2.0f;

    _colorBody = glm::vec3(0.545f, 0.271f, 0.075f);
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 1.5f, 0.5f, 1.0f );
    _rotateWingAngle = _PI / 2.0f;

    _colorNose = glm::vec3( 0.0f, 1.0f, 0.0f );
    _rotateNoseAngle = _PI / 2.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( 1.1f, 1.0f, 0.025f );
    _transProp = glm::vec3( 0.1f, 0.0f, 0.0f );

    _colorTail = glm::vec3( 1.0f, 1.0f, 0.0f );

    _colorMagicEffect = glm::vec3(0.5f, 0.0f, 0.5f);

    _arcBallCam = new ArcBallCam();
    _arcBallCam->setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    _arcBallCam->setTheta(0.0f);
    _arcBallCam->setPhi(1.57f);
}

GLfloat Vehicle::getPhi() {return mVehiclePhi; }
GLfloat Vehicle::getTheta() {return mVehicleTheta; }
glm::vec3 Vehicle::getPosition(){return mVehiclePosition;}

void Vehicle::setPhi( const GLfloat t ) { mVehiclePhi = t; }
void Vehicle::setTheta( const GLfloat t ) { mVehicleTheta = t; }
void Vehicle::setPosition(glm::vec3 pos) { mVehiclePosition = pos; }



void Vehicle::rotate(const GLfloat dTheta) {
    mVehicleTheta += dTheta;

}
void Vehicle::_drawMagicEffect(glm::vec3 position, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, position + glm::vec3(0.0f, -0.6f, 0.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.1f, 1.0f, 0.1f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorMagicEffect[0]);
    CSCI441::drawSolidCylinder(0.2f, 0.4f, 0.8f, 16, 16); // Elongated cylinder representing the magic effect
}

void Vehicle::spinHandle() {
    _propAngle += _propAngleRotationSpeed;
    if( _propAngle > _2PI ) _propAngle -= _2PI;
}

void Vehicle::_drawWagonBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::scale(modelMtx, glm::vec3(3.0f, 1.0f, 3.0f));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);
    CSCI441::drawSolidCube(0.5f);
}

// Drawing a wheel
void Vehicle::_drawWheel(glm::vec3 position, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, position);
    modelMtx = glm::scale(modelMtx, glm::vec3(0.5f, 0.5f, 0.5f));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);
    CSCI441::drawSolidCylinder(0.5f, 0.2f,0.4f, 16, 16);
}

// Drawing the wagon's handle
void Vehicle::_drawHandle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.2f, -0.8f));
    modelMtx = glm::scale(modelMtx, glm::vec3(1.0f, 0.1f, 0.1f));

    modelMtx = glm::rotate(modelMtx, _propAngle, CSCI441::Y_AXIS);
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);
    CSCI441::drawSolidCube(0.5f);
}

// Main drawing function
void Vehicle::drawWagon(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, -0.7f));
    modelMtx = glm::rotate(modelMtx, glm::radians(-90.0f), CSCI441::X_AXIS);

    _drawWagonBody(modelMtx, viewMtx, projMtx);

    glm::vec3 wheelPositions[] = {
            glm::vec3(-0.5f, -0.5f, 0.7f),
            glm::vec3(-0.5f, -0.5f, -0.7f),
            glm::vec3(0.5f, -0.5f, 0.7f),
            glm::vec3(0.5f, -0.5f, -0.7f)
    };
    for(const glm::vec3& wheelPosition : wheelPositions) {
        _drawWheel(wheelPosition, modelMtx, viewMtx, projMtx);
        _drawMagicEffect(wheelPosition, modelMtx, viewMtx, projMtx);
    }
    _drawHandle(modelMtx, viewMtx, projMtx);

    ///updateCam();
    ///updateCam is crashing program. Its the same as SkipShowers with same functions and variables.
}

void Vehicle::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}



ArcBallCam* Vehicle::getArcballCam() {
    return _arcBallCam;
}
void Vehicle::updateCam() {
    glm::vec3 cameraPosition = glm::vec3(_movementX - 5.0f * cos(_rotateHeroAngle),
                                         _movementY - sin(_rotateHeroAngle),
                                         5.0f);
    _arcBallCam->setPosition(cameraPosition);
    glm::vec3 lookAtPoint = glm::vec3(_movementX, 0.5f, _movementY);
    _arcBallCam->setLookAtPoint(lookAtPoint);
    _arcBallCam->recomputeOrientation();

    glm::vec3 fpCameraPosition = glm::vec3(_movementX,
                                           0.45f,
                                           _movementY);

    _firstPersonCam->setPosition(fpCameraPosition);
    glm::vec3 fpLookAtPoint = glm::vec3(_movementX-0.1f, 0.45f, _movementY);
    _firstPersonCam->setLookAtPoint(fpLookAtPoint);
    _firstPersonCam->setTheta(-_rotateHeroAngle-1.57f);
    _firstPersonCam->recomputeOrientation();

}

void Vehicle::turnRight() {
    _rotateHeroAngle -= 0.02f;
    if (_rotateHeroAngle <= -6.28f) {
        _rotateHeroAngle = 0.0f;
    }
}

void Vehicle::turnLeft() {
    _rotateHeroAngle += 0.02f;
    if (_rotateHeroAngle >= 6.28f) {
        _rotateHeroAngle = 0.0f;
    }
}
void Vehicle::moveForwards() {

    if (_movementX < 40.0f && _movementX > -40.0f && _movementY < 40.0f && _movementY > -40.0f) {
        _movementX -= cos(_rotateHeroAngle) / 100;
        _movementY += sin(_rotateHeroAngle) / 100;
    }
    else {  // reset character back to spawn if it reaches the world limit
        _movementX = 0.0f;
        _movementY = 0.0f;
    }
}
void Vehicle::moveBackwards() {

    if (_movementX < 40.0f && _movementX > -40.0f && _movementY < 40.0f && _movementY > -40.0f) {
        _movementX += cos(_rotateHeroAngle) / 100;
        _movementY -= sin(_rotateHeroAngle) / 100;
    }
    else {  // reset character back to spawn if it reaches the world limit
        _movementX = 0.0f;
        _movementY = 0.0f;
    }
}

CSCI441::FreeCam* Vehicle::getFirstPersonCam() const {
    return _firstPersonCam;
}
