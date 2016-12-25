#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name);
        
        void closeButtonPressed() override;
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };
}

#endif  // MAINWINDOW_H_INCLUDED
