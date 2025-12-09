#include "../include/Application/MACROS.h"
#include "../include/Application/Application.h"

int main(int argc, char *argv[]) {
#ifdef CLIENT
    Application app;
    app.init();
    app.run();
#endif
    return 0;
}
