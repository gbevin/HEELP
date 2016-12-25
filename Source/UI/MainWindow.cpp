#include "MainWindow.h"

#include "MainContentComponent.h"

using namespace heelp;

MainWindow::MainWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned(new MainContentComponent(), true);
    
    centreWithSize(getWidth(), getHeight());
    setVisible (true);
}

void MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
