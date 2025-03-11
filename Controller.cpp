#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"

Controller::Controller(Model& m,View& v) {
    model = m;
    view = v;
    filePathExists = false;
    initScenegraph();
}

Controller::Controller(Model& m,View& v, char* fp) {
    model = m;
    view = v;
    filePath = fp;
    filePathExists = true;
    initScenegraph();
}

void Controller::initScenegraph() {
    /*open up the file path (if given), otherwise display the castle (for testing purposes)
      INPUT THE RELATIVE PATH OF THE DESIRED SCENEGRAPH*/ 
    if(filePathExists){
        ifstream inFile(filePath);
        sgraph::ScenegraphImporter importer;
    
        IScenegraph *scenegraph = importer.parse(inFile);
        model.setScenegraph(scenegraph);
    }
    else {
        ifstream inFile("scenegraphmodels/castle.txt");
        sgraph::ScenegraphImporter importer;
    
        IScenegraph *scenegraph = importer.parse(inFile);
        model.setScenegraph(scenegraph);
    }

}

Controller::~Controller()
{
    
}

void Controller::run()
{
    sgraph::IScenegraph * scenegraph = model.getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib> > meshes = scenegraph->getMeshes();
    view.init(this,meshes);
    while (!view.shouldWindowClose()) {
        view.display(scenegraph);
        // when the left mouse button is pressed, take the cursor position and set it to the initial position
        if(pressed) {
            if (count == 0) {
                view.findMousePos(true);
                count++;
            }
            // if the button continues to be held, take the current position and find the difference from the initial
            view.findMousePos(false);
        }
        else {
            count = 0;
        }
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R){ // 'r' or 'R'
        //reset trackball
        view.resetTrackball();
    }
}

void Controller::onMouse(int button, int action, int mods)
{
    // set the "pressed" boolean accordingly when the left mouse button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        pressed = false;
    }
}

void Controller::reshape(int width, int height) 
{
    cout <<"Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}