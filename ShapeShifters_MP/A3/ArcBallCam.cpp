//
// Created by Grant on 10/6/2023.
//

#include "ArcBallCam.h"

ArcBallCam::ArcBallCam() {
    _minRadius = 0.5f;
    _maxRadius = 10.f;
    _aspectRatio = 1.0f;
    _fovy = 45.0f;
    _nearClipPlane = 0.001f;
    _farClipPlane = 1000.0f;
    mProjectionMatrix = glm::perspective(_fovy, _aspectRatio, _nearClipPlane, _farClipPlane);
    mCameraRadius = 1.0f;
}

void ArcBallCam::recomputeOrientation() {
    mCameraDirection.x = sin(mCameraTheta) * sin(mCameraPhi) * mCameraRadius;
    mCameraDirection.y = -cos(mCameraPhi) * mCameraRadius;
    mCameraDirection.z = -cos(mCameraTheta) * sin(mCameraPhi) * mCameraRadius;

    _updateArcBallCameraViewMatrix();
}

void ArcBallCam::moveBackward(GLfloat speed) {
    mCameraRadius += speed;
    _clampRadius();
    recomputeOrientation();
}

void ArcBallCam::moveForward(GLfloat speed) {
    mCameraRadius -= speed;
    _clampRadius();
    recomputeOrientation();
}

void ArcBallCam::_updateArcBallCameraViewMatrix() {
    setPosition(mCameraLookAtPoint + mCameraDirection); // could be because i never set a camera LaP and direction
    computeViewMatrix();
}

void ArcBallCam::_clampRadius() {
    mCameraRadius = glm::clamp(mCameraRadius, _minRadius, _maxRadius);
}
