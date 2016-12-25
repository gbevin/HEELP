#ifndef MAINCONTENTCOMPONENT_H_INCLUDED
#define MAINCONTENTCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class MainContentComponent : public Component
    {
    public:
        MainContentComponent();
        ~MainContentComponent();

        void paint(Graphics&) override;
        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    };
}

#endif  // MAINCONTENTCOMPONENT_H_INCLUDED
