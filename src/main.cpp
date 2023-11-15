
#include "App.h"

int main() {

    // create app
    App app;

    // draw loop
    while(app.isOpen()) {
        app.update();
        app.draw();
    }

    return 0;
}