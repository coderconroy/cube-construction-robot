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

	// Initialize OpenGL shape view timer
	// TODO: Review if this timer is necessary when other events trigger a shape view update
	openGLTimer = new QTimer(this);
	connect(openGLTimer, &QTimer::timeout, this, &DesignView::updateShapeView);
	openGLTimer->start(20); // Update feed every 20ms
}

void DesignView::updateShapeView()
{
	shapeView->update();
}