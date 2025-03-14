#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/GLScenegraphTextRenderer.h"
#include "VertexAttrib.h"


View::View() {

}

View::~View(){
    
}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Our Castle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
     glfwSetWindowUserPointer(window, (void *)callbacks);

    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow* window, int button, int action, int mods)
        {
            reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onMouse(button,action,mods);
        }
    );

    // glfwSetCursorPosCallback(window,
    //     [](GLFWwindow* window, double xpos, double ypos)
    //     {
    //         reinterpret_cast<Callbacks*>(glfwSetCursorPosCallback(window))->getMousePos(xpos,ypos);
    //     }
    // );

    // glfwSetWindowSizeCallback(window, 
    // [](GLFWwindow* window, int width,int height)
    // {
    //     reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    // });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    
    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    

	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);
    textRenderer = new sgraph::GLScenegraphTextRenderer();
    count = 0;
    rotateAmount.push(glm::mat4(1.0f));
    speed = 1.2f;
    previousTime = 0.0f;
    totalQuaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 cameraVector = glm::vec3(0.0f,300.0f,300.0f) - glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 rightVector = glm::cross(glm::vec3(0.0f,1.0f,0.0f), cameraVector);
    correctUp = glm::cross(cameraVector, rightVector);

    cameraMode = GLOBAL;

}


void View::display(sgraph::IScenegraph *scenegraph) {
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    float time = (float)glfwGetTime();  
    float timeDiff = time - previousTime;
    previousTime = time;

    // Roll the drone until it makes a complete loop (360 degree)
    if (rolling) {
        rollingTotal += timeDiff*2;
        if (rollingTotal >= 6.28319f) {
            rolling = false;
            rollingTotal = 0;
        }
        sgraph::RotateTransform* droneRot = dynamic_cast<sgraph::RotateTransform*>(scenegraph->getRoot()->getNode("rx-drone"));
        droneRot->changeRotation(timeDiff*2);
    }
    
    sgraph::RotateTransform* propellorRotationOne = dynamic_cast<sgraph::RotateTransform*>(scenegraph->getRoot()->getNode("r-propellor-1"));
    sgraph::RotateTransform* propellorRotationTwo = dynamic_cast<sgraph::RotateTransform*>(scenegraph->getRoot()->getNode("r-propellor-2"));
    sgraph::RotateTransform* rotateDrone = dynamic_cast<sgraph::RotateTransform*>(scenegraph->getRoot()->getNode("r-drone"));
    sgraph::TranslateTransform* moveDrone = dynamic_cast<sgraph::TranslateTransform*>(scenegraph->getRoot()->getNode("t-drone"));

    // Set default drone position
    if(count < 1){
        droneOriginalPos = moveDrone->getTranslate();
    }

    //Propellor rotation
    propellorRotationOne->changeRotation(timeDiff * speed);
    propellorRotationTwo->changeRotation(timeDiff * speed);

    //Drone rotation
    rotateDrone->setRotationAxis(glm::axis(totalQuaternion));
    rotateDrone->setRotation(glm::angle(totalQuaternion));

    //Drone movement to relative forward direction
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotateDrone->getAngleInRadians(), rotateDrone->getRotationAxis());
    glm::vec4 forwardVector(1.0f, 0.0f, 0.0f, 1.0f); // Default forward vector
    glm::vec4 newForwardVector = rotation * forwardVector;
    moveDrone->moveForwardBackward(glm::vec3(newForwardVector.x, newForwardVector.y, newForwardVector.z) * droneMovement);
    droneMovement = 0.0f;

    //Drone position/rotation reset
    if(resetDrone){
        moveDrone->setTranslate(droneOriginalPos);
        rotateDrone->resetAngle();
        totalQuaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        resetDrone = false;
    }

    modelview.push(glm::mat4(1.0));

    /*Different cameras based on cameraMode. Trackball is only usable in GLOBAL camera mode
    */
    if (cameraMode == GLOBAL) {
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f,300.0f,300.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
        // rotate by the amount that the cursor travels in the x and y coordinates
        stack<glm::mat4> temp_stack = rotateAmount;
        while (!temp_stack.empty()) {
            glm::mat4 temp_mat = temp_stack.top();
            modelview.top() = modelview.top() * temp_mat;
            temp_stack.pop();
        }
    }
    else if (cameraMode == CHOPPER) {
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f, 450.0f, 300.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
        modelview.top() = modelview.top() * glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f,1.0f,0.0f));
    }
    else {
        glm::vec3 dronePos = moveDrone->getTranslate();
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(dronePos.x,dronePos.y + 10.0f,dronePos.z),glm::vec3(dronePos.x + 100.0f,dronePos.y + 10.0f,dronePos.z),glm::vec3(0.0f,1.0f,0.0f));
        modelview.top() = modelview.top() * glm::inverse(
            glm::translate(glm::mat4(1.0), moveDrone->getTranslate()) * 
            rotation * 
            glm::inverse(glm::translate(glm::mat4(1.0), moveDrone->getTranslate())));
    }
    
    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);
    // print the text renderer only once
    if(count < 1) {
        scenegraph->getRoot()->accept(textRenderer);
        count++;
    }
    
    modelview.pop();
    glFlush();
    program.disable();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// determine the amount by which the model will rotate based on cursor movement
void View::findMousePos(bool init)
{
    if (cameraMode == GLOBAL) {
        // get the cursor position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        /*set the initial position if instructed to do so, else calculate the distance travelled by the cursor
        and set the amount by which the model will rotate to that distance (converted to radians within rotation matrix)*/
        if (init) {
            prevpos[0] = (float)xpos;
            prevpos[1] = (float)ypos;
        }
        else {
            float diffx = (float)xpos - prevpos[0];
            float diffy = (float)ypos - prevpos[1];
            rotateAmount.push(glm::rotate(glm::mat4(1.0f), glm::radians(diffx), correctUp));
            rotateAmount.push(glm::rotate(glm::mat4(1.0f), glm::radians(diffy), glm::vec3(1.0f, 0.0f, 0.0f)));
            // prep for next position
            prevpos[0] = (float)xpos;
            prevpos[1] = (float)ypos;
        }
    }
}

// reset the rotation
void View::resetTrackball()
{
    while (!rotateAmount.empty()) {
        rotateAmount.pop();
    }
    rotateAmount.push(glm::mat4(1.0f));
}

// increase drone propellor speed
void View::increasePropellorSpeed() {
    speed += 1.0f;
}

// decrease drone propellor speed
void View::decreasePropellorSpeed() {
    if(speed > 1.1f) {
        speed -= 1.0f;
    }
}

// trigger drone sideways rolling
void View::sidewaysRoll() {
    rolling = true;
}

// move drone backward
void View::moveDroneBackward() {
    droneMovement = -0.5f * speed;
}

// move drone forward
void View::moveDroneForward() {
    droneMovement = 0.5f * speed;
}

// Accumulate drone rotation
void View::moveDroneFace(int direction){
    glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis(0.0f, 0.0f, 1.0f);
    switch(direction){
        case 0: // RIGHT
            totalQuaternion *= glm::angleAxis(-0.1f, yAxis);
            break;
        case 1: // LEFT
            totalQuaternion *= glm::angleAxis(0.1f, yAxis);
            break;
        case 2: // DOWN
            totalQuaternion *= glm::angleAxis(0.1f, zAxis);
            break;
        case 3: // UP
            totalQuaternion *= glm::angleAxis(-0.1f, zAxis);
            break;
    }
}

// Reset drone position and rotation
void View::resetDronePosition() {
    resetDrone = true;
}

// Change camera view option
void View::changeCam(int cam) {
    if (cam == 1) {
        cameraMode = GLOBAL;
    }
    else if (cam == 2) {
        cameraMode = CHOPPER;
    }
    else if (cam == 3) {
        cameraMode = FPS;
    }
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}





