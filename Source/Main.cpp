/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

// I'm going to be doing a fair amount of cut-and-paste from a few JUCE tutorials.
// Wish me luck. I sort of understand some of this.

// My logic is going to be in a separate header file. Why is logic in header files?
// Good thing to research, I guess.
#include <JuceHeader.h>
#include "VivNoiseHeader.h"

/*
 OK, so someting as simple as adding a header file:
 I had to go back to the Projuicer to add "VivNoiseHeader.h" - simply
 adding it by hand didn't work, the IDE didn't see it.
 
 When I used the Projucer, it created the file and added some code to it. That code
 does not appear in the SimpleSynthNoiseTutorial. So I'm going to kill it...
 */


//==============================================================================

// nice try, automated code generator...
//int main (int argc, char* argv[])
//{
//
//    // ..your code goes here!
//
//
//    return 0;
//}

// oops gotta go back to the Projucer and add some more modules...
// ... aaaaand that fixed it...

class Application : public juce::JUCEApplication
{
// add public and private parts
public:
    // here comes some C&P:
    Application() = default;
    const juce::String getApplicationName() override       { return "VivNoise"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow ("VivNoise", std::make_unique<MainContentComponent>(), *this));
    }

    void shutdown() override                         { mainWindow = nullptr; }

    
private:
    class MainWindow : public juce::DocumentWindow
    {
        // more public and private parts
        public:
        MainWindow (const juce::String& name, std::unique_ptr<juce::Component> c, JUCEApplication& a)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                .findColour (ResizableWindow::backgroundColourId),
                              juce::DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);

           #if JUCE_ANDROID || JUCE_IOS
            setContentOwned (new SafeAreaComponent { std::move (c) }, true);
            setFullScreen (true);
           #else
            setContentOwned (c.release(), true);
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

       #if JUCE_ANDROID || JUCE_IOS
        class SafeAreaComponent : public juce::Component
        {
        public:
            explicit SafeAreaComponent (std::unique_ptr<Component> c)
                : content (std::move (c))
            {
                addAndMakeVisible (*content);
            }

            void resized() override
            {
                if (const auto* d = Desktop::getInstance().getDisplays().getDisplayForRect (getLocalBounds()))
                    content->setBounds (d->safeAreaInsets.subtractedFrom (getLocalBounds()));
            }

        private:
            std::unique_ptr<Component> content;
        };

        void parentSizeChanged() override
        {
            if (auto* c = getContentComponent())
                c->resized();
        }
       #endif


        private:
            JUCEApplication& app;

            //==============================================================================
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)

        
    };
    
    // this one is an important little bit - The AI tels me:
    //  "C++ smart pointer that manages a dynamically allocated object with exclusive ownership semantics"
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION (Application)
