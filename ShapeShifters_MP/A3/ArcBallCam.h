//
// Created by Grant on 10/6/2023.
//

#ifndef A3_ARCBALLCAM_H
#define A3_ARCBALLCAM_H

#include <CSCI441/Camera.hpp>

class ArcBallCam final : public CSCI441::Camera {
public:
    ArcBallCam();
    void recomputeOrientation() final;
    void moveForward(GLfloat speed) final;
    void moveBackward(GLfloat speed) final;
private:
    void _updateArcBallCameraViewMatrix();
    void _clampRadius();
    GLfloat _minRadius;
    GLfloat _maxRadius;
    GLfloat _fovy;
    GLfloat _aspectRatio;
    GLfloat _nearClipPlane;
    GLfloat _farClipPlane;
};


#endif //A3_ARCBALLCAM_H
