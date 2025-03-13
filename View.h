#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include <glm/gtc/quaternion.hpp>

#include <stack>
using namespace std;


class View
{
    typedef enum {GLOBAL, CHOPPER, FPS} TypeOfCamera;

public:
    View();
    ~View();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes);
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();
    void findMousePos(bool init);
    void resetTrackball();
    void decreasePropellorSpeed();
    void increasePropellorSpeed();
    void sidewaysRoll();
    void moveDroneForward();
    void moveDroneBackward();
    void moveDroneFace(int direction);
    void resetDronePosition();
    void changeCam(int cam);

private: 

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    sgraph::SGNodeVisitor *textRenderer;
    int count;
    float prevpos[2];
    stack<glm::mat4> rotateAmount;
    glm::vec3 correctUp;
    float speed;
    float previousTime;
    bool rolling = false;
    float rollingTotal = 0.0f;
    float droneMovement = 0.0f;
    TypeOfCamera cameraMode;
    glm::vec3 droneOriginalPos = glm::vec3(0.0f, 0.0f, 0.0f);
    bool resetDrone = false;
    float droneTotalLR = 0.0f;
    float droneTotalUD = 0.0f;
    glm::quat totalQuaternion;
};

#endif