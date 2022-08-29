

/*
Copyright 2022
J. M. Hayes
Motas Electronics Limited

This file is part of MotasEdit.

MotasEdit is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
 either version 3 of the License, or (at your option) any later version.

MotasEdit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MotasEdit.
 If not, see <https://www.gnu.org/licenses/>.


*/


/*
  ==============================================================================

    CustomLook.cpp


  ==============================================================================
*/


#include "./Headers/CustomLook.h"
#include "./Headers/utilities.h"
#include "../binaryResources/MotasEditResources.h"

//==============================================================================
CustomLook::CustomLook(int type)
: type(type)
{
	//knobImageRed = ImageFileFormat::loadFrom(MotasEditResources::knobRed_png, MotasEditResources::knobRed_pngSize);
	knobImageYellow = ImageFileFormat::loadFrom(MotasEditResources::knobRedReal_png, MotasEditResources::knobRedReal_pngSize);
	//knobImageYellow = ImageFileFormat::loadFrom(MotasEditResources::knobYellow_png, MotasEditResources::knobYellow_pngSize);
	knobImageRed = ImageFileFormat::loadFrom(MotasEditResources::knobYellowReal_png, MotasEditResources::knobYellowReal_pngSize);
	knobImageBlur = ImageFileFormat::loadFrom(MotasEditResources::knobBlur_png, MotasEditResources::knobBlur_pngSize);

	knobImage = nullptr;
}



void CustomLook::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                              bool isMouseOverButton, bool isButtonDown)
   {

      auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                                 .withMultipliedAlpha      (button.isEnabled() ? 0.9f : 0.5f);

	   if (isButtonDown || isMouseOverButton)
		   baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.1f);

	   auto width  = button.getWidth()  - 1.0f;
	   auto height = button.getHeight() - 1.0f;

	   //baseColour = Colour(0xFF, 0x00, 0x00);

	   if (button.getToggleState() || isButtonDown )
	      	   baseColour = Utilities::buttonYellow; //


	   if (width > 0 && height > 0)
	   {
		//   g.setGradientFill (ColourGradient::vertical (baseColour, 0.0f,
		//												baseColour.darker (0.1f), height));

		//   g.fillRect (button.getLocalBounds());

		auto flatOnLeft   = button.isConnectedOnLeft();
	   auto flatOnRight  = button.isConnectedOnRight();
	   auto flatOnTop    = button.isConnectedOnTop();
	   auto flatOnBottom = button.isConnectedOnBottom();


		   auto cornerSize = cornerSizeBase * width *4.0f;//jmin (3.0f, jmin (width, height) * 0.45f);

			if (cornerSize > cornerMax)
				cornerSize =cornerMax;


		  auto lineThickness = cornerSize    * 0.1f;
		  auto halfThickness = lineThickness * 0.5f;

		  Path outline;
		  outline.addRoundedRectangle (0.5f + halfThickness, 0.5f + halfThickness, width - lineThickness, height - lineThickness,
									   cornerSize, cornerSize,
									   ! (flatOnLeft  || flatOnTop),
									   ! (flatOnRight || flatOnTop),
									   ! (flatOnLeft  || flatOnBottom),
									   ! (flatOnRight || flatOnBottom));

		  auto outlineColour = button.findColour (button.getToggleState() ? TextButton::textColourOnId
																		  : TextButton::textColourOffId);

		  g.setColour (baseColour);
		  g.fillPath (outline);

		  if (! button.getToggleState())
		  {
			  g.setColour (outlineColour);
			  g.strokePath (outline, PathStrokeType (lineThickness));
		  }


	   }



}

void CustomLook::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
   {

	/*
       auto diameter = jmin (width, height) - 4.0f;
       auto radius = (diameter / 2.0f) * std::cos (MathConstants<float>::pi / 4.0f);
       auto centreX = x + width  * 0.5f;
       auto centreY = y + height * 0.5f;
       auto rx = centreX - radius;
       auto ry = centreY - radius;
       auto rw = radius * 2.0f;
       auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
       bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

       auto baseColour = slider.isEnabled() ? slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.8f : 1.0f)
                                            : Colour (0x80808080);

       Rectangle<float> r (rx, ry, rw, rw);
       auto transform = AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY());

       auto x1 = r.getTopLeft()   .getX();
       auto y1 = r.getTopLeft()   .getY();
       auto x2 = r.getBottomLeft().getX();
       auto y2 = r.getBottomLeft().getY();

       transform.transformPoints (x1, y1, x2, y2);

       g.setGradientFill (ColourGradient (baseColour, x1, y1,
                                          baseColour.darker (0.1f), x2, y2,
                                          false));

       Path knob;
       knob.addRectangle (r);
       g.fillPath (knob, transform);

       Path needle;
       auto r2 = r * 0.1f;
       needle.addRectangle (r2.withPosition ({ r.getCentreX() - (r2.getWidth() / 2.0f), r.getY() }));

       g.setColour (slider.findColour (Slider::rotarySliderOutlineColourId));
       g.fillPath (needle, AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY()));*/

//	bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
	// auto baseColour = slider.isEnabled() ? slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.8f : 1.0f)
	//                                            : Colour (0x80808080);



	// set to true to use png image of knob instead of vectors
	const bool useImage = !true;

	auto radius = jmin (width / 2, height / 2) - 4.0f;

	auto diameter = radius * 2;
	//radius *= 0.7;
	//radius *= 0.9;
	auto centreX = x + width  * 0.5f;
	auto centreY = y + height * 0.5f;
	auto rx = centreX - radius;
	auto ry = centreY - radius;
	auto rw = radius * 2.0f;
	auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);


	// fill
	g.setColour (Colours::black);

	if (!useImage)
		g.fillEllipse (rx, ry, rw, rw);

	auto centreXImage = centreX;
	auto centreYImage = centreY;



	//g.drawImageAt (image, 0, 0);
/*
	Path myPath;
	Point<float> centre(centreX, centreY);
	myPath.addPolygon(centre, 12, radius, 0); // MathConstants<float>::pi / 6.0f
	auto transform = AffineTransform::rotation (angle, centreX, centreY);
	myPath.applyTransform (transform);
	g.fillPath(myPath);

*/

	radius *= 0.8f;
	rx = centreX - radius;
	ry = centreY - radius;
	rw = radius * 2.0f;


	//g.setColour (Colour (0x20, 0x20, 0x20));
	//g.fillEllipse (rx, ry, rw, rw);

	auto baseColour = slider.isEnabled() ? Colour (0x40, 0x40, 0x40) : Colour (0x60, 0x60, 0x60) ;



	g.setColour(baseColour);

	Path myPath;
	juce::Point<float> centre(centreX, centreY);
	myPath.addPolygon(centre, 14, radius, 0); // MathConstants<float>::pi / 6.0f
	auto transform = AffineTransform::rotation (angle, centreX, centreY);
	myPath.applyTransform (transform);
	g.fillPath(myPath);

	// outline
	//g.setColour (Colours::red);
	//g.drawEllipse (rx, ry, rw, rw, 1.0f);

	radius *= 1.1f;
	Path p;
	auto pointerLength = radius * 0.1f;
	auto pointerThickness = jmax(2.0f, radius*0.24f);

	p.startNewSubPath(-pointerThickness * 0.25f, -radius);
	p.lineTo(-pointerThickness * 0.5f, -pointerLength);
	p.lineTo(pointerThickness * 0.5f, -pointerLength);
	p.lineTo(pointerThickness * 0.25f, -radius);
	p.closeSubPath();
	//p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));


	// pointer









	// draw the pointer
	if (!useImage)
	{
		if (slider.isEnabled())
		{
			if (type == 0)
					g.setColour (Colours::white);
				else
					g.setColour (Colours::red);
		} else
		{
			if (type == 0)
					g.setColour (Colours::grey);
				else
					g.setColour (Colours::grey);
		}
		g.fillPath (p);
	} else
	{




		if (type == 0)
			knobImage = &knobImageRed;
		else
			knobImage = &knobImageYellow;
		if (knobImage->isValid())
		{
			//g.drawImage (knobImage,       centreX -w/4, centreY-w/4, w, w,
			//	                          0, 0, 255,255,	   false) ;
			AffineTransform af;

			// move to centre of image
			af = af.translated(-knobImage->getWidth()/2.0f, -knobImage->getHeight()/2.0f);

			// rotate by knob angle
			af = af.rotated(angle, 0, 0);
			af = af.scaled((float) diameter/knobImage->getWidth(), (float) diameter/knobImage->getWidth(), 0,0);
			//auto w = centreXImage ;

			// move to the knob position
			af = af.translated(centreXImage, centreYImage);

			g.drawImageTransformed(*knobImage, af, false);

			/*
			af = AffineTransform::translation(-knobImage.getWidth()/2, -knobImage.getHeight()/2);


			af = af.scaled(diameter/knobImage.getWidth(), 0.8*diameter/knobImage.getWidth(), 0,0);

			af = af.translated(centreXImage, centreYImage);


			g.drawImageTransformed(knobImageBlur, af, false);
			*/
		}
	}






	// LookAndFeel_V4::drawRotarySlider (g, x, y,  width,  height,  sliderPos, rotaryStartAngle,  rotaryEndAngle,  slider);

   }




void CustomLook::drawDrawableButton(Graphics& , DrawableButton& ,
		bool , bool )
{

	//DBG("Custom look");



}

void CustomLook::drawLinearSlider(Graphics& g, int x, int y, int width,
		int height, float sliderPos, float minSliderPos, float maxSliderPos,
		const Slider::SliderStyle sliderStyle, Slider& slider)

{

	if (slider.isBar())
	{

		//DBG("x:" + String(x) + " y:" + String(y) + " SliderPos:" + String(sliderPos) + " height:" + String(height));

		if (type == 1)
		{
			if (slider.getValue() < 0)
			{
				g.setColour (Colour(0x70,0x0,0x0));


				g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
										  : Rectangle<float> (x + 0.5f, height/2.0f, width - 1.0f, y + (-height/2.0f + sliderPos)));

			}
			else
			{
				 g.setColour (slider.findColour (Slider::trackColourId));
				 g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
														  : Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height/2.0f - sliderPos)));
			}
		}
		else
		{

			LookAndFeel_V4::drawLinearSlider(g,x, y, width, height, sliderPos, minSliderPos, maxSliderPos,sliderStyle, slider);

		}


	}else
	{
		 LookAndFeel_V4::drawLinearSlider(g,x, y, width, height, sliderPos, minSliderPos, maxSliderPos,sliderStyle, slider);
	}

}

void CustomLook::drawButtonText(Graphics& g, TextButton& button,
		bool , bool isButtonDown)
{

	  Font font (getTextButtonFont (button, button.getHeight()));
    g.setFont (font);

    if (button.getToggleState() || isButtonDown)
    	g.setColour(Colours::black);
    else
    {
    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                            : TextButton::textColourOffId)
                       .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
    }

    const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
    const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = roundToInt (font.getHeight() * 0.6f);
    const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText (button.getButtonText(),
                          leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                          Justification::centred, 2);




}

void CustomLook::drawTextEditorOutline(Graphics& g, int width, int height,
		TextEditor& textEditor)
{
	//DBG("Draw text edit");
	auto cornerSize = cornerSizeBase * width *4.0f;
	if (cornerSize > cornerMax)
		cornerSize = cornerMax;
	 if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
	    {
	        if (textEditor.isEnabled())
	        {
	            if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
	            {
	                g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
	                //g.drawRect (0, 0, width, height, 2);
	                g.drawRoundedRectangle(0, 0, (float) width, (float) height, cornerSize,  2.0f);
	            }
	            else
	            {
	                g.setColour (textEditor.findColour (TextEditor::outlineColourId));
	                //g.drawRect (0, 0, width, height);
	                g.drawRoundedRectangle(0, 0, (float) width,(float)  height,(float)  cornerSize,  2.0f);
	            }
	        }
	    }


}

void CustomLook::fillTextEditorBackground(Graphics& g, int width, int height,
		TextEditor& textEditor)
{
	auto cornerSize = cornerSizeBase * 4.0f * width ;

	if (cornerSize > cornerMax)
		cornerSize = cornerMax;

	    if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) != nullptr)
	    {
	        g.setColour (textEditor.findColour (TextEditor::backgroundColourId));
	        g.fillRect (0, 0, width, height);

	        g.setColour (textEditor.findColour (TextEditor::outlineColourId));
	        g.drawHorizontalLine (height - 1, 0.0f, static_cast<float> (width));
	    }
	    else
	    {
	        //LookAndFeel_V2::fillTextEditorBackground (g, width, height, textEditor);
	        //g.fill (textEditor.findColour (TextEditor::backgroundColourId));
	        g.setColour (textEditor.findColour (TextEditor::backgroundColourId));
	        g.fillRoundedRectangle(0, 0, (float) width, (float)height, (float) cornerSize);
	    }

}

void CustomLook::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (1, 1,
                     box.getWidth() - 2,
                     box.getHeight() - 2);

    label.setFont (getComboBoxFont (box));
}



void CustomLook::drawComboBox (Graphics& g, int width, int height, bool,
                                   int, int, int, int, ComboBox& box)
{
    //auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
	auto cornerSize = cornerSizeBase * width *8.0f;

	if (cornerSize > cornerMax)
		cornerSize =cornerMax;

    Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (1.0f, 1.0f), cornerSize, 1.0f);

   // Rectangle<int> arrowZone (width - 30, 0, 20, height);
   // Path path;
  //  path.startNewSubPath (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
  //  path.lineTo (static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
  //  path.lineTo (arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

   // g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
  //  g.strokePath (path, PathStrokeType (2.0f));

    g.setColour (Colour(0xA0,0,0).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));

    g.drawLine((float)width - 3, 3, (float)width - 3, (float) height - 3, 3);



}




void CustomLook::drawTabButton(TabBarButton& button, Graphics& g,
		bool isMouseOver, bool isMouseDown)
{
	const Rectangle<int> activeArea (button.getActiveArea());

	    const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();

	    const Colour bkg (button.getTabBackgroundColour());





	    if (button.getToggleState())
	    {
	        //g.setColour (bkg);
	        g.setColour(Utilities::buttonYellow);
	    }
	    else
	    {
	        juce::Point<int> p1, p2;

	        switch (o)
	        {
	            case TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
	            case TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
	            case TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
	            case TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
	            default:                              jassertfalse; break;
	        }

	        g.setGradientFill (ColourGradient (bkg.brighter (0.2f), p1.toFloat(),
	                                           bkg.darker (0.1f),   p2.toFloat(), false));
	    }

	    g.fillRect (activeArea);

	    g.setColour (button.findColour (TabbedButtonBar::tabOutlineColourId));

	    Rectangle<int> r (activeArea);

	    if (o != TabbedButtonBar::TabsAtBottom)   g.fillRect (r.removeFromTop (1));
	    if (o != TabbedButtonBar::TabsAtTop)      g.fillRect (r.removeFromBottom (1));
	    if (o != TabbedButtonBar::TabsAtRight)    g.fillRect (r.removeFromLeft (1));
	    if (o != TabbedButtonBar::TabsAtLeft)     g.fillRect (r.removeFromRight (1));

	    const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

	    Colour col (bkg.contrasting().withMultipliedAlpha (alpha));

	    if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
	    {
	        TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId
	                                                               : TabbedButtonBar::tabTextColourId;

	        if (bar->isColourSpecified (colID))
	            col = bar->findColour (colID);
	        else if (isColourSpecified (colID))
	            col = findColour (colID);
	    }

	    if (button.getToggleState())
		{

	    	col = Colours::black;
		}

	    const Rectangle<float> area (button.getTextArea().toFloat());

	    float length = area.getWidth();
	    float depth  = area.getHeight();

	    if (button.getTabbedButtonBar().isVertical())
	        std::swap (length, depth);

	    TextLayout textLayout;
	    createTabTextLayout (button, length, depth, col, textLayout);

	    AffineTransform t;

	    switch (o)
	    {
	        case TabbedButtonBar::TabsAtLeft:   t = t.rotated (MathConstants<float>::pi * -0.5f).translated (area.getX(), area.getBottom()); break;
	        case TabbedButtonBar::TabsAtRight:  t = t.rotated (MathConstants<float>::pi *  0.5f).translated (area.getRight(), area.getY()); break;
	        case TabbedButtonBar::TabsAtTop:
	        case TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
	        default:                            jassertfalse; break;
	    }

	    g.addTransform (t);
	    textLayout.draw (g, Rectangle<float> (length, depth));



}
