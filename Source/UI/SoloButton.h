/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_4E6F4F551FF396C2__
#define __JUCE_HEADER_4E6F4F551FF396C2__

//[Headers]     -- You can add your own extra header files here --
#include "../../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SoloButton  : public Component
{
public:
    //==============================================================================
    SoloButton ();
    ~SoloButton();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    bool getSoloed();
    void setSoloed(bool value);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    void mouseUp(const MouseEvent& event) override;
    
    bool soloed_;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> labelSolo_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoloButton)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_4E6F4F551FF396C2__
