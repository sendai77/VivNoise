//
//  VivNoiseHeader.h
//  VivNoise
//
//  Created by jmj on 10/31/26.
//

#ifndef VivNoiseHeader_h
#define VivNoiseHeader_h

#pragma once

// OK, so here we get to define a class called MainContentComponent
// (Main.cpp was barking errors that it could not find MainContentComponent
// so this will fix it)

class MainContentComponent :    public juce::AudioAppComponent,
                                public juce::Slider::Listener
{
    // confession - a whole lotta C&P from the other tutorial just took place
public:
    
    int labelWidth = 60;
    
    MainContentComponent()
    {
        // hey! here's where you can set the app window's initial size...
        setSize (600, 600);
        int windowWidth = getWidth();
        // int windowHeight = getHeight();
        // ... and add sme audio, hey?
        setAudioChannels (0, 2); // no inputs, two outputs
        
        //  adding the volume slider!
        
        
        addAndMakeVisible (volumeLabel);
        volumeLabel.setText ("Volume", juce::dontSendNotification);
        volumeLabel.attachToComponent (&volumeSlider, true);
        
        addAndMakeVisible(volumeSlider);
        volumeSlider.setRange(0.0, 1.0);
        
        // labelWidth = volumeLabel.getWidth();
        
        DBG ("volumeLabel.getWidth() " + std::to_string(labelWidth));
        DBG ("windowWidth - (2 * labelWidth) " + std::to_string(windowWidth - (2 * labelWidth)));
        volumeSlider.setBounds(labelWidth + 20, 20, windowWidth - (2 * labelWidth), 40);
        
        
        //  adding the filter slider!
//        addAndMakeVisible(filterSlider);
//        filterSlider.setRange(0.0, 0.25);
//        filterSlider.setBounds(filterLabel.getWidth() + 20, 80, windowWidth - 20, 40);
//        addAndMakeVisible (filterLabel);
//        filterLabel.setText ("Cutoff", juce::dontSendNotification);
//        filterLabel.attachToComponent (&volumeSlider, true);

    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        juce::String message;
        message << "Preparing to play audio...\n";
        message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
        message << " sampleRate = " << sampleRate;
        juce::Logger::getCurrentLogger()->writeToLog (message);
        
        // filter stuff!
        lpFilterSpec.sampleRate = sampleRate;
        lpFilterSpec.maximumBlockSize = samplesPerBlockExpected;
        // example said this:
        // mySpec.numChannels = getTotalNumOutputChannels();
        lpFilterSpec.numChannels = 2; // cheating - hard-coded because I know the answer
        
        lpFilter.prepare(lpFilterSpec);
        lpFilter.reset();
        
        lpFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        lpFilter.setCutoffFrequency(1000.0f);
        lpFilter.setResonance(0.0f);
    }
    
    // part of slider adding:
    void resized() override
    {
//        auto sliderLeft = 120;
//        volumeSlider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
        int windowWidth = getWidth();
        volumeSlider.setBounds (labelWidth + 20, 20, windowWidth - (2 * labelWidth), 40);
    }

    void sliderValueChanged (juce::Slider* slider) override
    {
        // I think some magic has to happen here...
        
    }
    
    void releaseResources() override
    {
        juce::Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
        lpFilter.reset();
    }

    // here's where the audio magic happens:
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto level = (float) volumeSlider.getValue();
        
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            // Get a pointer to the start sample in the buffer for this audio output channel
            auto* buffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

            // Fill the required number of samples with noise between -0.125 and +0.125
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                auto noise = random.nextFloat() * 2.0f - 1.0f;
                buffer[sample] = noise * level;
            }
            // maybe the filter code goes... here?
            juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);

            lpFilter.process(context);
                
        }
    }

private:
    juce::Random random;
    // OK! Let's add a slider!
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    
    juce::Slider filterSlider;
    juce::Label filterLabel;
    
    // from https://jucestepbystep.wordpress.com/2b-make-a-filter/
    juce::dsp::StateVariableTPTFilter<float> lpFilter;
    juce::dsp::ProcessSpec lpFilterSpec;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

#endif /* VivNoiseHeader_h */

