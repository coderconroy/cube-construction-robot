#include "DesignView.h"

DesignView::DesignView(QWidget* parent): QWidget(parent) 
{
	// Initialize OpenGL view for the 3D shapes
	shapeView = new OpenGLView(parent);
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	shapeView->setFormat(format);

	// Initialize design view layout
	baseLayout = new QVBoxLayout();
	baseLayout->addWidget(shapeView);

	setLayout(baseLayout);
}