#include "MinimalOgre.h"

int main(int argc, char** argv) {
    MinimalOgre app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
