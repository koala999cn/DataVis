#include "KsImApp.h"


int main(int, char**)
{
    auto& app = KsImApp::singleton();

    if (!app.init(1024, 768, "DataVis"))
        return 1;

    app.run();

    app.shutdown();

    return 0;
}
