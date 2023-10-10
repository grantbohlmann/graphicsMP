//
// Created by Grant on 10/6/2023.
//

#include "SkipShowers.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>
#include <iostream>

SkipShowers::SkipShowers( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _legAngle = 0.0f;
    _rotateHeroAngle = 0.0f;
    _legDirectionBool = true;
    _movementX = 0.0f; _movementY = 0.0f;
    _cameraTheta = 0.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _bodyColor = glm::vec3( 1.0f, 0.0f, 0.0f ); // red
    _bodyScale = glm::vec3( 1.0f, 0.85f, 1.0f );

    _headColor = glm::vec3( 1.0f, 0.0f, 0.0f ); // red
    _headScale = glm::vec3( 1.0f, 0.85f, 1.0f );

    _legColor = glm::vec3( 0.75f, 0.0f, 0.0f ); // darker red
    _legScale = glm::vec3( 1.0f, 1.0f, 1.0f );

    _bagColor = glm::vec3( 0.75f, 0.0f, 0.0f ); // darker red
    _bagScale = glm::vec3( 1.0f, 1.0f, 2.0f );

    _eyesColor = glm::vec3(0.6f, 0.6f, 0.6f);   // gray
    _eyesScale = glm::vec3(0.9f, 0.5f, 1.0f);

    _arcBallCam = new ArcBallCam();
    _arcBallCam->setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    _arcBallCam->setTheta(0.0f);
    _arcBallCam->setPhi(1.57f);

    _firstPersonCam = new CSCI441::FreeCam();
    _firstPersonCam->setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    _firstPersonCam->setTheta(_cameraTheta);
    _firstPersonCam->setPhi(1.57f);
}

void SkipShowers::drawHero( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    modelMtx = glm::translate(modelMtx, glm::vec3(_movementX, _movementY, 0.0f));
    modelMtx = glm::rotate( modelMtx, -_rotateHeroAngle-1.57f, CSCI441::Z_AXIS );
    _drawBody(modelMtx, viewMtx, projMtx);        // the body
    _drawHead(modelMtx, viewMtx, projMtx);        // the head
    _drawLeg(true, modelMtx, viewMtx, projMtx);  // the left leg
    _drawLeg(false, modelMtx, viewMtx, projMtx); // the right leg
    _drawBag(modelMtx, viewMtx, projMtx);        // the bag
    _drawEyes(modelMtx, viewMtx, projMtx);   // the eyes

    updateCam();
}

void SkipShowers::walkHero() {
    if (_legDirectionBool) {
        _legAngle += 0.1f;
        if (_legAngle > (_PI / 3)) {_legDirectionBool = false;}
    }
    else {
        _legAngle -= 0.1f;
        if (_legAngle < -(_PI / 3)) {_legDirectionBool = true;}
    }
}

void SkipShowers::turnHero() {
    _rotateHeroAngle -= .1;
    if( _rotateHeroAngle < 0.0f ) _rotateHeroAngle += (2*_PI);
}

void SkipShowers::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _bodyScale );
    modelMtx = glm::rotate(modelMtx, _PI/2, CSCI441::X_AXIS);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_bodyColor[0]);

    CSCI441::drawSolidCylinder(0.1f, 0.1f, 0.25f, 32, 32);
}

void SkipShowers::_drawLeg(bool isLeftLeg, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _legScale );
    if (isLeftLeg) {
        modelMtx = glm::translate(modelMtx, glm::vec3(-0.05f, 0.0f, 0.25f));
        modelMtx = glm::rotate(modelMtx, _legAngle, CSCI441::X_AXIS);
    }
    else {
        modelMtx = glm::translate(modelMtx, glm::vec3(0.05f, 0.0f, 0.25f));
        modelMtx = glm::rotate(modelMtx, -_legAngle, CSCI441::X_AXIS);
    }
    modelMtx = glm::rotate( modelMtx, _PI/2, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_legColor[0]);

    CSCI441::drawSolidCylinder(0.05f, 0.03f, 0.07f, 16, 16);
    CSCI441::drawSolidSphere(0.05f, 16, 16);
}

void SkipShowers::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _headScale );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_headColor[0]);

    CSCI441::drawSolidSphere(0.1f, 32, 32);
}

void SkipShowers::_drawBag(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _bagScale );
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.085f, 0.05f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_bagColor[0]);

    CSCI441::drawSolidCube( 0.1f );
}

void SkipShowers::_drawEyes(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale(modelMtx, _eyesScale);
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, -0.085f, 0.02f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_eyesColor[0]);

    CSCI441::drawSolidSphere(0.1f, 32, 32);
}

void SkipShowers::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

void SkipShowers::turnRight() {
    _rotateHeroAngle -= 0.02f;
    if (_rotateHeroAngle <= -6.28f) {
        _rotateHeroAngle = 0.0f;
    }
}

void SkipShowers::turnLeft() {
    _rotateHeroAngle += 0.02f;
    if (_rotateHeroAngle >= 6.28f) {
        _rotateHeroAngle = 0.0f;
    }
}

void SkipShowers::moveForwards() {
    walkHero();
    if (_movementX < 40.0f && _movementX > -40.0f && _movementY < 40.0f && _movementY > -40.0f) {
        _movementX -= cos(_rotateHeroAngle) / 100;
        _movementY += sin(_rotateHeroAngle) / 100;
    }
    else {  // reset character back to spawn if it reaches the world limit
        _movementX = 0.0f;
        _movementY = 0.0f;
    }
}

void SkipShowers::moveBackwards() {
    walkHero();
    if (_movementX < 40.0f && _movementX > -40.0f && _movementY < 40.0f && _movementY > -40.0f) {
        _movementX += cos(_rotateHeroAngle) / 100;
        _movementY -= sin(_rotateHeroAngle) / 100;
    }
    else {  // reset character back to spawn if it reaches the world limit
        _movementX = 0.0f;
        _movementY = 0.0f;
    }
}

/*
 * Getters
 */
GLfloat SkipShowers::getMovementX() const {
    return _movementX;
}
GLfloat SkipShowers::getMovementY() const {
    return _movementY;
}
ArcBallCam* SkipShowers::getArcballCam() const {
    return _arcBallCam;
}
CSCI441::FreeCam* SkipShowers::getFirstPersonCam() const {
    return _firstPersonCam;
}
GLfloat SkipShowers::getHeroRotationAngle() const {
    return _rotateHeroAngle;
}

/*
 * More of a helper function, used in the drawHero function
 */
void SkipShowers::updateCam() {
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
