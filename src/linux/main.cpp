#include <cstdio>
#include "UIDlg/MainWindow.h"

using namespace std;

int main() {
    MainWindow mainWindow;
    if(!mainWindow.CreateFullScreenWindow()){
        printf("Can't Create X11 Window...\n");
        return 0;
    }
    mainWindow.ShowWindow();
    mainWindow.MessageLoop();
    return 0;
}