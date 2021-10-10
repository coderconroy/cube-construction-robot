#include "DesignView.h"
#include <glm/glm.hpp>;

DesignView::DesignView(QWidget* parent): QWidget(parent) 
{
	// Initialize cube world model
	cubeWorldModel = new CubeWorldModel(this);

	// Layer 1 Row 1
	cubeWorldModel->insertCube(glm::vec3(1.8, -2, -5));
	cubeWorldModel->insertCube(glm::vec3(0.6, -2, -5));
	cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -5));
	cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -5));

	// Layer 1 Row 2
	cubeWorldModel->insertCube(glm::vec3(1.8, -2, -6.2));
	cubeWorldModel->insertCube(glm::vec3(0.6, -2, -6.2));
	cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -6.2));
	cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -6.2));

	// Layer 1 Row 3
	cubeWorldModel->insertCube(glm::vec3(1.8, -2, -7.4));
	cubeWorldModel->insertCube(glm::vec3(0.6, -2, -7.4));
	cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -7.4));
	cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -7.4));

	// Layer 1 Row 4
	cubeWorldModel->insertCube(glm::vec3(1.8, -2, -8.6));
	cubeWorldModel->insertCube(glm::vec3(0.6, -2, -8.6));
	cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -8.6));
	cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -8.6));

	// Layer 2 Row 1
	cubeWorldModel->insertCube(glm::vec3(1.2, -1, -5.6));
	cubeWorldModel->insertCube(glm::vec3(0.0, -1, -5.6));
	cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -5.6));

	// Layer 2 Row 2
	cubeWorldModel->insertCube(glm::vec3(1.2, -1, -6.8));
	cubeWorldModel->insertCube(glm::vec3(0.0, -1, -6.8));
	cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -6.8));

	// Layer 2 Row 3
	cubeWorldModel->insertCube(glm::vec3(1.2, -1, -8.0));
	cubeWorldModel->insertCube(glm::vec3(0.0, -1, -8.0));
	cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -8.0));

	// Layer 3 Row 1
	cubeWorldModel->insertCube(glm::vec3(0.6, 0, -6.2));
	cubeWorldModel->insertCube(glm::vec3(-0.6, 0, -6.2));

	// Layer 3 Row 2
	cubeWorldModel->insertCube(glm::vec3(0.6, 0, -7.4));
	cubeWorldModel->insertCube(glm::vec3(-0.6, 0, -7.4));

	// Layer 4 Row 1
	cubeWorldModel->insertCube(glm::vec3(0, 1, -6.8));

	// Initialize OpenGL view for the 3D shapes
	shapeView = new OpenGLView(parent);
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	shapeView->setFormat(format);
	shapeView->setCubes(cubeWorldModel->getCubes());

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