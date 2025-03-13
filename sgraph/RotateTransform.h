#ifndef _ROTATETRANSFORM_H_
#define _ROTATETRANSFORM_H_

#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {

/**
 * This class represents a specific type of transformation: rotation
 * 
 */
    class RotateTransform: public TransformNode {
        protected:
            float angleInRadians;
            glm::vec3 axis;

            ParentSGNode *copyNode() {
                return new RotateTransform(angleInRadians,axis[0],axis[1],axis[2],name,scenegraph);
            }

        public:
            RotateTransform(float angleInRadians,float ax,float ay,float az,const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->angleInRadians = angleInRadians;
                    this->axis = glm::vec3(ax,ay,az);
                    glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                    setTransform(transform);
            }


            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                return visitor->visitRotateTransform(this);
            }

            glm::vec3 getRotationAxis() {
                return axis;
            }

            float getAngleInRadians() {
                return angleInRadians;
            }

            // Reset rotation angle to 0
            void resetAngle() {
                this->angleInRadians = 0.0f;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                setTransform(transform);
            }
            
            // Rotate this transformation by the given angle
            void changeRotation(float newAngle) {
                this->angleInRadians += newAngle;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                setTransform(transform);
            } 

            // Set rotation axis to given axis
            void setRotationAxis(glm::vec3 newAxis) {
                this->axis = newAxis;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                setTransform(transform);
            }

            // Set rotation to given angle
            void setRotation(float angle) {
                this->angleInRadians = angle;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                setTransform(transform);
            }

    };
}

#endif