#include "application.h"

using namespace TinyServer;

int main(int argc, char** argv) 
{
    Application app;
    if(app.init(argc, argv)) 
    {
        return app.run();
    }
    return 0;
}