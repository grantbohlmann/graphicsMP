//
// Created by Grant on 10/6/2023.
//

#ifndef LAB05_SKIPSHOWERS_H
#define LAB05_SKIPSHOWERS_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "ArcBallCam.h"

class SkipShowers {
public:
    SkipShowers(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation);

    void drawHero(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);

    void walkHero();
    void turnHero();

    // movement functions
    void moveForwards();
    void moveBackwards();
    void turnRight();
    void turnLeft();

    GLfloat getMovementX() const;
    GLfloat getMovementY() const;
    ArcBallCam* getCam() const;
    GLfloat getHeroRotationAngle() const;

    void updateCam();

private:
    ArcBallCam* _arcBallCam;

    GLfloat _legAngle;  // use positive angle for one leg and negative angle for the other for animation
    GLfloat _rotateHeroAngle;   // used to determine which direction hero is facing
    GLboolean _legDirectionBool;

    GLfloat _movementX;
    GLfloat _movementY;

    GLuint _shaderProgramHandle;

    struct ShaderProgramUniformLocations {
        GLint mvpMtx;
        GLint normalMtx;
        GLint materialColor;
    } _shaderProgramUniformLocations;
    // body
    glm::vec3 _bodyColor;
    glm::vec3 _bodyScale;
    // head
    glm::vec3 _headColor;
    glm::vec3 _headScale;
    // legs
    glm::vec3 _legColor;
    glm::vec3 _legScale;
    // bag
    glm::vec3 _bagColor;
    glm::vec3 _bagScale;
    // eyes
    glm::vec3 _eyesColor;
    glm::vec3 _eyesScale;

    // pi constant for rotation
    const GLfloat _PI = glm::pi<float>();

    void _drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawLeg(bool isLeftLeg, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawBag(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawEyes(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};

#endif //LAB05_SKIPSHOWERS_H