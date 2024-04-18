#pragma once

#include <JuceHeader.h>
#include "XyPad.h"


namespace Gui
{	

    

	class XyPad : public Component, Slider::Listener
	{
	public:
		enum Axis { X, Y, DiagonalTopLeftToBottomRight, DiagonalTopRightToBottomLeft };
		class Thumb : public Component
		{
		public:
			Thumb();
			void mouseDown(const MouseEvent& event) override;
			void mouseDrag(const MouseEvent& event) override;
			void paint(Graphics& g) override;
			std::function<void(Point<double>)> moveCallback;
		private:
			ComponentBoundsConstrainer constrainer;
			ComponentDragger dragger;
		};

		XyPad();

		void registerSlider(Slider* slider, Axis axis);
		void deregisterSlider(Slider* slider);
        
       
		
		void resized() override;
        void paint(Graphics& g) override;
        void boringButtonClicked();
      
	
    private:
		void sliderValueChanged(Slider* slider) override;
        
        juce::Image backgroundImage;
        
        void initializeBackgroundImage();
       
        
        
      
		Thumb thumb;
        std::vector<Slider*> xSliders, ySliders, diagonalSliders, diagonalTopRightSliders;
        std::vector<Slider*> allSliders;
		static constexpr int thumbWidth = 35;

		std::mutex mutex;
        bool isReversed = true;
        
        
	};
}
