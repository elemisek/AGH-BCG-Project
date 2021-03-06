﻿#include "RotarySolidsMainFrame.h"

RotarySolidsMainFrame::RotarySolidsMainFrame( wxWindow* parent ) : MainFrame( parent )
{
	xSlider->SetRange(0, 360);
	ySlider->SetRange(0, 360);
	zSlider->SetRange(0, 360);

	setDefaultSliders();
}

void RotarySolidsMainFrame::LeftPanelOnLeftDClick( wxMouseEvent& event )
{
	setDefaultSliders();
	generateShape();
}

void RotarySolidsMainFrame::LeftPanelOnUpdateUI( wxUpdateUIEvent& event )
{
	Repaint(false);
}

void RotarySolidsMainFrame::xLabelOnLeftDClick( wxMouseEvent& event )
{
	xSlider->SetValue(180);
	rotateByXYZ();
}

void RotarySolidsMainFrame::xyzSliderUpdated( wxScrollEvent& event )
{
	rotateByXYZ();
}

void RotarySolidsMainFrame::yLabelOnLeftDClick( wxMouseEvent& event )
{
	ySlider->SetValue(180);
	rotateByXYZ();
}

void RotarySolidsMainFrame::zLabelOnLeftDClick( wxMouseEvent& event )
{
	zSlider->SetValue(180);
	rotateByXYZ();
}

void RotarySolidsMainFrame::OpenPropertiesWindowButtonOnButtonClick( wxCommandEvent& event )
{
	propertiesFrame = new RotarySolidsSelectionFrame(this);
	propertiesFrame->Show();
}

void RotarySolidsMainFrame::SaveToFileButtonOnButtonClick( wxCommandEvent& event )
{
	wxInitAllImageHandlers();

	wxClientDC DC(LeftPanel);
	wxBufferedDC buffDC(&DC);

	int width, height;
	LeftPanel->GetSize(&width, &height);

	wxBitmap bitmapSave(width, height, -1);
	wxMemoryDC memory(bitmapSave);

	memory.Blit(0, 0, width, height, &buffDC, 0, 0);
	memory.SelectObject(bitmapSave);
	std::string fileName = "rysunek" + std::to_string(fileNum++) + ".png";
	bitmapSave.SaveFile(fileName, wxBITMAP_TYPE_PNG);
}

void RotarySolidsMainFrame::parallelViewResetOnButtonClick( wxCommandEvent& event )
{
	setDefaultSliders();
	parallelView = false;
}

void RotarySolidsMainFrame::parallelView1OnButtonClick( wxCommandEvent& event )
{
	setDefaultSliders();
	parallelView = true;
	xSlider->SetValue(0);
}

void RotarySolidsMainFrame::parallelView2OnButtonClick( wxCommandEvent& event )
{
	setDefaultSliders();
	parallelView = true;
	xSlider->SetValue(180);
}

void RotarySolidsMainFrame::parallelView3OnButtonClick( wxCommandEvent& event )
{
	setDefaultSliders();
	parallelView = true;
	xSlider->SetValue(270);
}


void RotarySolidsMainFrame::drawShape(wxBufferedDC& buffDC, int width, int height, bool discardZ)
{
	buffDC.SetPen(wxColor(73, 135, 235));
	buffDC.SetBrush(wxColor(161, 177, 201));

	std::vector<Segment> triangles;
	triangles.resize(2 * data.size() * (solidRoundnessLevel + 1));
	wxPoint shapePoints[3];
	size_t k;
	k = 0;

	#pragma omp parallel for
	for (size_t i = 1; i < data.size(); i++)
	{	
		for (size_t j = 1; j < data[0].size(); j++)
		{
			triangles[k] = Segment(data[i - 1][j - 1], data[i][j - 1], data[i][j]);
			triangles[k + 1] = Segment(data[i - 1][j - 1], data[i - 1][j], data[i][j]);
			k += 2;
		}
	}

	std::sort(triangles.begin(), triangles.end(), [](Segment s1, Segment s2){ return s1.sumZ() > s2.sumZ(); });

	#pragma omp parallel for
	for (size_t i = 0; i < triangles.size(); i++)
	{
		shapePoints[0] = Point3DtoPoint2D(triangles[i].mA, width, height, discardZ);
		shapePoints[1] = Point3DtoPoint2D(triangles[i].mB, width, height, discardZ);
		shapePoints[2] = Point3DtoPoint2D(triangles[i].mC, width, height, discardZ);

		buffDC.DrawPolygon(3, shapePoints);
	}
}

void RotarySolidsMainFrame::setDefaultSliders()
{
	xSlider->SetValue(180);
	ySlider->SetValue(180);
	zSlider->SetValue(180);
}

void RotarySolidsMainFrame::setPolygonSides(int sides)
{
	polygonSides = sides;
}

void RotarySolidsMainFrame::generateShape()
{
	xCoord /= 10.;
	zCoord /= 10.;

	switch (shape)
	{
		case Shape::square:
		{
			if (xCoord)
			{
				//	|	A---B
				//	|	|   |
				//	|	D---C
				resizeData(5);
				data[4][0](xCoord, 0., 0.25 + zCoord);

			}
			else
			{
				//	|A---B
				//	||   |
				//	|D---C
				resizeData(4);
			}

			data[0][0](xCoord, 0., 0.25 + zCoord);
			data[1][0](0.25 + xCoord, 0., 0.25 + zCoord);
			data[2][0](0.25 + xCoord, 0., 0. + zCoord);
			data[3][0](xCoord, 0., zCoord);

			break;
		}
		case Shape::rectangle:
		{
			if(xCoord)
			{
				//	|	A-------B
				//	|	|       |
				//	|	D-------C
				resizeData(5);
				data[4][0](xCoord, 0., 0.25 + zCoord);
			}
			else
			{
				//	|A-------B
				//	||       |
				//	|D-------C
				resizeData(4);
			}

			data[0][0](xCoord, 0., 0.25 + zCoord);
			data[1][0](0.45 + xCoord, 0., 0.25 + zCoord);
			data[2][0](0.45 + xCoord, 0., zCoord);
			data[3][0](xCoord, 0., zCoord);

			break;
		}
		case Shape::triangle:
		{
			if (xCoord)
			{
				//	|	A
				//	|	| \
				//  |	|  \
				//	|	B---C
				resizeData(4);
				data[3][0](xCoord, 0., zCoord);
			}
			else
			{
				//	|A
				//	|| \
				//  ||  \
				//	|B---C
				resizeData(3);
			}

			data[0][0](xCoord, 0., zCoord);
			data[1][0](0.25 + xCoord, 0., zCoord);
			data[2][0](xCoord, 0., 0.45 + zCoord);

			break;
		}
		case Shape::circle:
		{	
			//	|   ----
			//  | /	  	 \
			//	||        |
			//  | \		 /
			//	|   ----
			resizeData(solidRoundnessLevel + 1);

			data[0][0](0.2, 0., 0.2);
			circleNextPoints(data[0], solidRoundnessLevel);

			break;
		}
		case Shape::polygon:
		{
			// |   ---
			// | /     \
			// | \     /
			// |   ---
			resizeData(polygonSides + 1);

			data[0][0](0.2, 0., 0.2);
			circleNextPoints(data[0], polygonSides);

			break;
		}
		case Shape::parabola:
		{
			//  ||		   |
			//	| \       /
			//  |  \	 /
			//	|    ---
			resizeData(solidRoundnessLevel + 1);

			double step = 0.03;
			for (int i = 0; i < solidRoundnessLevel; i++)
			{
				data[i][0](step + xCoord, data[0][0].mY, (step * step) + zCoord);
				step += 0.02;
			}
			data[solidRoundnessLevel][0] = data[0][0];

			break;
		}
		case Shape::line:
		{
			// |    /
			// |   /
			// |  /
			// | /
			resizeData(2);

			data[0][0](xCoord, 0., zCoord);
			data[1][0](0.35 + xCoord, 0., 0.25 + zCoord);

			break;
		}
		case Shape::sombrero:
		{
			resizeData(5);

			data[0][0](xCoord, 0., zCoord);
			data[1][0](0.15 + xCoord, 0., 0.10 + zCoord);
			data[2][0](0.25 + xCoord, 0., 0.35 + zCoord);
			data[3][0](0.50 + xCoord, 0., 0.35 + zCoord);
			data[4][0](0.52 + xCoord, 0., 0.25 + zCoord);

			break;
		}
	}

	#pragma omp parallel for
	for (size_t i = 0; i < data.size(); i++)
	{
		nextPoints(data[i]);
	}

	dataWithoutRotation.insert(dataWithoutRotation.begin(), data.begin(), data.end());
}

void RotarySolidsMainFrame::circleNextPoints(std::vector<Point>& circle, int sides)
{
	double phi = std::atan2(data[0][0].mZ, data[0][0].mX);
	double angleStep = 2 * M_PI / sides;
	double radius = std::sqrt(std::pow(data[0][0].mX, 2) + std::pow(data[0][0].mZ, 2));

	#pragma omp parallel for
	for (size_t i = 1; i < sides; i++)
	{
		data[i][0](xCoord + radius * std::cos(phi + i * angleStep), data[0][0].mY, zCoord + radius * std::sin(phi + i * angleStep));
	}

	data[0][0](xCoord + 0.2, 0., zCoord + 0.2);
	data[sides][0] = data[0][0];
}

void RotarySolidsMainFrame::setShape(Shape shape)
{
	this->shape = shape;
}

void RotarySolidsMainFrame::Repaint(bool discardZ)
{
	wxClientDC DC(LeftPanel);
	wxBufferedDC buffDC(&DC);

	int width, height;
	LeftPanel->GetSize(&width, &height);
	LeftPanel->SetBackgroundColour(wxColor(242, 235, 230));
	buffDC.Clear();
	buffDC.SetClippingRegion(wxRect(0, 0, width, height));

	rotateByXYZ();
	drawShape(buffDC, width, height, parallelView);

	Update();
}

void RotarySolidsMainFrame::setSolidRoundnessLevel(int num)
{
	solidRoundnessLevel = num;
}

wxPoint RotarySolidsMainFrame::Point3DtoPoint2D(Point p, int width, int height, bool discardZ)
{
	if (discardZ)
	{
		return wxPoint((width / 2.) + (width * p.mX), (height / 2.) + (height * p.mY));
	}
	else
	{
		return wxPoint((width / 2.) + ((width / 2.) * p.mX / std::abs(1. + p.mZ)), (height / 2.) + ((height / 2.) * p.mY / std::abs(1. + p.mZ)));
	}
}

void RotarySolidsMainFrame::resizeData(int v)
{
	data.resize(v);
	dataWithoutRotation.resize(v);

	#pragma omp parallel for
	for (size_t i = 0; i < v; i++)
	{
		data[i].resize(solidRoundnessLevel + 1);
		dataWithoutRotation[i].resize(solidRoundnessLevel + 1);
	}
}

void RotarySolidsMainFrame::rotateByXYZ()
{
	double phi, r, angleChange;

	angleChange = (xSlider->GetValue() - 180.) * (M_PI / 180.);

	#pragma omp parallel for
	for (size_t i = 0; i < data.size(); i++)
	{
		#pragma omp parallel for
		for (size_t j = 0; j < data[0].size(); j++)
		{
			phi = std::atan2(dataWithoutRotation[i][j].mY, dataWithoutRotation[i][j].mZ);
			r = std::sqrt(std::pow(dataWithoutRotation[i][j].mY, 2) + std::pow(dataWithoutRotation[i][j].mZ, 2));
			
			data[i][j](dataWithoutRotation[i][j].mX, std::sin(phi + angleChange)* r, std::cos(phi + angleChange)* r);
		}
	}

	angleChange = (ySlider->GetValue() - 180.) * (M_PI / 180.);

	#pragma omp parallel for
	for (size_t i = 0; i < data.size(); i++)
	{
		for (size_t j = 0; j < data[0].size(); j++)
		{
			phi = std::atan2(data[i][j].mX, data[i][j].mZ);
			r = std::sqrt(std::pow(data[i][j].mX, 2) + std::pow(data[i][j].mZ, 2));

			data[i][j](std::sin(phi + angleChange)* r, data[i][j].mY, std::cos(phi + angleChange)* r);
		}
	}

	angleChange = (zSlider->GetValue() - 180.) * (M_PI / 180.);

	#pragma omp parallel for
	for (size_t i = 0; i < data.size(); i++)
	{
		for (size_t j = 0; j < data[0].size(); j++)
		{
			phi = std::atan2(data[i][j].mY, data[i][j].mX);
			r = std::sqrt(std::pow(data[i][j].mX, 2) + std::pow(data[i][j].mY, 2));

			data[i][j](std::cos(phi + angleChange)* r, std::sin(phi + angleChange)* r, data[i][j].mZ);
		}
	}
}

void RotarySolidsMainFrame::setXCoord(int x)
{
	xCoord = x;
}

void RotarySolidsMainFrame::setZCoord(int z)
{
	zCoord = z;
}

void RotarySolidsMainFrame::nextPoints(std::vector<Point>& data)
{
	double phi = std::atan2(data[0].mY, data[0].mX);
	double angleStep = 2. * M_PI / solidRoundnessLevel;
	double radius = std::sqrt(std::pow(data[0].mX, 2) + std::pow(data[0].mY, 2));

	#pragma omp parallel for
	for (size_t i = 1; i < solidRoundnessLevel; i++)
	{
		data[i](radius* std::cos(phi + i * angleStep), radius* std::sin(phi + i * angleStep), data[0].mZ);
	}
	data[solidRoundnessLevel] = data[0];
}