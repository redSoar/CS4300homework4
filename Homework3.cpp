//! [code]

#include <glad/glad.h>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <iostream>
int main(int argc,char *argv[]) {
    Model model;
    View view;
    if(argc > 1) {
        cout << "filePath : " << argv[1] << endl;
        Controller controller(model,view, argv[1]);
        controller.run();
    }
    else {
        Controller controller(model,view);
        controller.run();
    }
    


}

//! [code]
