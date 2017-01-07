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

#ifndef __JUCE_HEADER_FE5E16B28238F40E__
#define __JUCE_HEADER_FE5E16B28238F40E__

//[Headers]     -- You can add your own extra header files here --
#include "../../JuceLibraryCode/JuceHeader.h"
//[/Headers]

#include "VolumeFader.h"
#include "PanFader.h"
#include "MuteButton.h"
#include "SoloButton.h"


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ChannelFader  : public Component,
                      public ChangeListener,
                      public LabelListener
{
public:
    //==============================================================================
    ChannelFader ();
    ~ChannelFader();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    bool getMuted();
    void setMuted(bool value);
    bool getSoloed();
    void setSoloed(bool value);
    float getVolume();
    void setVolume(float value);
    float getPan();
    void setPan(float value);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void labelTextChanged (Label* labelThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    void editorShown(Label* label, TextEditor& editor) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> labelVolume_;
    ScopedPointer<Label> labelPan_;
    ScopedPointer<Label> labelName_;
    ScopedPointer<VolumeFader> faderVolume_;
    ScopedPointer<PanFader> faderPan_;
    ScopedPointer<MuteButton> buttonMute_;
    ScopedPointer<SoloButton> buttonSolo_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelFader)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_FE5E16B28238F40E__
