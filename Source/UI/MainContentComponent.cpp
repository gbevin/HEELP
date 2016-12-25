#include "MainContentComponent.h"

using namespace heelp;

MainContentComponent::MainContentComponent()
{
    setSize(600, 400);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xff001F36));

    g.setFont(Font(16.0f));
    g.setColour(Colours::white);
    g.drawText("Hub for Expressive Electronic Live Performance", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
}
