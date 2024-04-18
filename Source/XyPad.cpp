#include "XyPad.h"
#include <random>


namespace Gui
{


XyPad::Thumb::Thumb()
{
    constrainer.setMinimumOnscreenAmounts(thumbWidth, thumbWidth, thumbWidth, thumbWidth);
}

void XyPad::Thumb::mouseDown(const MouseEvent& event)
{
    dragger.startDraggingComponent(this, event);
}

void XyPad::Thumb::mouseDrag(const MouseEvent& event)
{
    dragger.dragComponent(this, event, &constrainer);
    if (moveCallback)
        moveCallback(getPosition().toDouble());
}

void XyPad::Thumb::paint(Graphics& g)
{
    // Draw the rocket emoji image
    Image rocketImage = ImageCache::getFromMemory(BinaryData::rocket_png, BinaryData::rocket_pngSize);
    g.drawImage(rocketImage, getLocalBounds().toFloat());
//    g.setColour(Colours::whitesmoke);
//    g.fillEllipse(getLocalBounds().toFloat());
    
}

XyPad::XyPad()
{
    addAndMakeVisible(thumb);
    thumb.moveCallback = [&](Point<double> position)
    {
        const std::lock_guard<std::mutex> lock(mutex);
        const auto bounds = getLocalBounds().toDouble();
        const auto w = static_cast<double>(thumbWidth);
        for (auto* slider : xSliders)
        {
            if (isReversed)
                slider->setValue(jmap(position.getX(), bounds.getWidth() - w, 0.0, slider->getMinimum(), slider->getMaximum()));
            else
                slider->setValue(jmap(position.getX(), 0.0, bounds.getWidth() - w, slider->getMinimum(), slider->getMaximum()));
        }
        for (auto* slider : ySliders)
        {
            if (isReversed)
                slider->setValue(jmap(position.getY(), 0.0, bounds.getHeight() - w, slider->getMinimum(), slider->getMaximum()));
            else
                slider->setValue(jmap(position.getY(), bounds.getHeight() - w, 0.0, slider->getMinimum(), slider->getMaximum()));
        }
        for (auto* slider : diagonalSliders)
        {
            if (isReversed)
                slider->setValue(jmap(position.getX() + position.getY(), bounds.getWidth() + bounds.getHeight() - 2 * w, 0.0, slider->getMinimum(), slider->getMaximum()));
            else
                slider->setValue(jmap(position.getX() + position.getY(), 0.0, bounds.getWidth() + bounds.getHeight() - 2 * w, slider->getMinimum(), slider->getMaximum()));
            
            
  
        }
        for (auto* slider : diagonalTopRightSliders)
        {
            if (isReversed)
                slider->setValue(jmap(bounds.getWidth() - position.getX() + position.getY(), bounds.getWidth() + bounds.getHeight() - 2 * w, 0.0, slider->getMinimum(), slider->getMaximum()));
            else
                slider->setValue(jmap(bounds.getWidth() - position.getX() + position.getY(), 0.0, bounds.getWidth() + bounds.getHeight() - 2 * w, slider->getMinimum(), slider->getMaximum()));
        
            
        }
        
        repaint();
    };

    initializeBackgroundImage();


   
}


void XyPad::registerSlider(Slider* slider, Axis axis)
{
    slider->addListener(this);
    const std::lock_guard<std::mutex> lock(mutex);
    if (axis == X)
    {
        xSliders.push_back(slider);
        
    }
    if (axis == Y)
    {
        ySliders.push_back(slider);
        
    }
    if (axis == DiagonalTopLeftToBottomRight) // Register sliders for diagonal axis
    {
        diagonalSliders.push_back(slider);
        
    }
    if (axis == DiagonalTopRightToBottomLeft)
    {
        diagonalTopRightSliders.push_back(slider);
        
        
    }
    
    
}




void XyPad::deregisterSlider(Slider* slider)
{
    slider->removeListener(this);
    const std::lock_guard<std::mutex> lock(mutex);
    
    xSliders.erase(std::remove(xSliders.begin(), xSliders.end(), slider), xSliders.end());
    ySliders.erase(std::remove(ySliders.begin(), ySliders.end(), slider), ySliders.end());
    diagonalSliders.erase(std::remove(diagonalSliders.begin(), diagonalSliders.end(), slider), diagonalSliders.end());
    diagonalTopRightSliders.erase(std::remove(diagonalTopRightSliders.begin(), diagonalTopRightSliders.end(), slider), diagonalTopRightSliders.end()); // Remove from the new diagonal axis
    
}

void XyPad::resized()
{
    thumb.setBounds(getLocalBounds().withSizeKeepingCentre(thumbWidth, thumbWidth));
    if (!xSliders.empty())
        sliderValueChanged(xSliders[0]);
    if (!ySliders.empty())
        sliderValueChanged(ySliders[0]);
    if (!diagonalSliders.empty()) // Handle diagonal sliders on resize
        sliderValueChanged(diagonalSliders[0]);
    if(!diagonalTopRightSliders.empty())
        sliderValueChanged(diagonalTopRightSliders[0]);
    
    
    
    
}




void XyPad::initializeBackgroundImage()
{
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::space_jpg, BinaryData::space_jpgSize);
}


void XyPad::paint(Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    
    
    
    //    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::space_jpg, BinaryData::space_jpgSize);
    //    g.drawImage(backgroundImage, bounds.toFloat());
    if (backgroundImage.isNull()) {
        initializeBackgroundImage();
    }
    
    g.drawImage(backgroundImage, bounds);
    
//        g.setGradientFill(ColourGradient{ Colours::black.brighter(0.2f), bounds.getTopLeft(), Colours::black.brighter(0.1f), bounds.getBottomLeft(), false });
//        g.fillRoundedRectangle(bounds, 10);
    
    const auto r = thumbWidth / 2.f;
    const auto thumbX = thumb.getX() + r;
    const auto thumbY = thumb.getY() + r;
    
    g.setColour(Colours::lightseagreen);
    g.drawLine(Line<float> { {0.f, thumbY}, { bounds.getWidth(), thumbY } });
    
    g.setColour(Colours::lightgoldenrodyellow);
    g.drawLine(Line<float> { {thumbX, 0.f}, { thumbX, bounds.getHeight() } });
    
    
    Path circle;
    circle.addEllipse(thumb.getBoundsInParent().toFloat());
    const DropShadow dropShadow;
    dropShadow.drawForPath(g, circle);
    
    
 
    
}
void XyPad::boringButtonClicked()
{
    isReversed = !isReversed;
    
    
}


void XyPad::sliderValueChanged(Slider* slider)
{
    if (thumb.isMouseOverOrDragging(false))
        return;
    
    const auto bounds = getLocalBounds().toDouble();
    const auto w = static_cast<double>(thumbWidth);
    const auto isXAxisSlider = std::find(xSliders.begin(), xSliders.end(), slider) != xSliders.end();
    const auto isYAxisSlider = std::find(ySliders.begin(), ySliders.end(), slider) != ySliders.end();
    const auto isDiagonalSlider = std::find(diagonalSliders.begin(), diagonalSliders.end(), slider) != diagonalSliders.end(); // Check if it's a diagonal slider
    const auto isDiagonalTopRightSlider = std::find(diagonalTopRightSliders.begin(), diagonalTopRightSliders.end(), slider) != diagonalTopRightSliders.end(); // Check if it's a diagonal top right to bottom left slider
    
    if (isXAxisSlider)
    {
        if (isReversed) {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getWidth() - w, 0.0),
                                     thumb.getY());
        } else {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getWidth() - w),
                                     thumb.getY());
        }
    }
    else if (isYAxisSlider)
    {
        if (isReversed) {
            thumb.setTopLeftPosition(
                                     thumb.getX(),
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getHeight() - w));
        } else {
            thumb.setTopLeftPosition(
                                     thumb.getX(),
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getHeight() - w, 0.0));
        }
    }
    else if (isDiagonalSlider) // Handle the diagonal slider case
    {
        if (isReversed) {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getWidth() - w, 0.0),
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getHeight() - w, 0.0));
        } else {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getWidth() - w),
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getHeight() - w));
        }
    }
    else if (isDiagonalTopRightSlider)
    {
        if (isReversed) {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getWidth() - w), // Invert the direction for top right to bottom left
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getHeight() - w, 0.0));
        } else {
            thumb.setTopLeftPosition(
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getWidth() - w, 0.0),
                                     jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getHeight() - w));
        }
    }
    
    repaint();
}
}
