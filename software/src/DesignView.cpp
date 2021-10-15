#include "DesignView.h"
#include <glm/glm.hpp>;

DesignView::DesignView(QWidget* parent): QWidget(parent) 
{
	// Initialize cube world model
	cubeWorldModel = new CubeWorldModel(this);

	// Initialize 3D shape design controls
	insertCube = new QPushButton("Insert Cube");
	removeCube = new QPushButton("Remove Cube");
	saveModel = new QPushButton("Save Model");
	loadModel = new QPushButton("Load Model");
	clearModel = new QPushButton("Clear Model");
	cubeList = new QListWidget();

	cubeList->setMaximumWidth(300);
	removeCube->setEnabled(false);

	// Initialize control signal connections
	connect(insertCube, &QPushButton::clicked, this, &DesignView::insertCubeClicked);
	connect(removeCube, &QPushButton::clicked, this, &DesignView::removeCubeClicked);
	connect(saveModel, &QPushButton::clicked, this, &DesignView::saveModelClicked);
	connect(loadModel, &QPushButton::clicked, this, &DesignView::loadModelClicked);
	connect(clearModel, &QPushButton::clicked, this, &DesignView::clearModelClicked);
	connect(cubeList, &QListWidget::itemSelectionChanged, this, &DesignView::cubeListSelectionChange);

	// Initialize OpenGL view for the 3D shapes
	shapeView = new OpenGLView(parent);
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	shapeView->setFormat(format);
	shapeView->setCubes(cubeWorldModel->getCubes());

	// Propagate log signal
	connect(shapeView, &OpenGLView::log, this, &DesignView::log);

	// Initialize controls layout
	controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(insertCube);
	controlsLayout->addWidget(removeCube);
	controlsLayout->addWidget(saveModel);
	controlsLayout->addWidget(loadModel);
	controlsLayout->addWidget(clearModel);
	controlsLayout->addWidget(cubeList);

	// Initialize design view layout
	baseLayout = new QHBoxLayout();
	baseLayout->addLayout(controlsLayout);
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

void DesignView::insertCubeClicked()
{
	// Create new cube in cubeWorldModel
	const Cube* cube = cubeWorldModel->insertCube(glm::vec3(0, 0.5, 0));

	// Add cube to cube list widget
	QString cubeDescription = "Cube " + QString::number(++lastCubeReference);
	cubeMap.insert(cubeDescription, cube);
	cubeList->addItem(cubeDescription);
}

void DesignView::removeCubeClicked()
{
	// Get selected cube from cube list
	QString cubeDescription = cubeList->currentItem()->text();
	const Cube* cube = cubeMap.value(cubeDescription);

	// Remove selected cube from cube world model, cube list and cube map
	cubeMap.remove(cubeDescription);
	cubeList->takeItem(cubeList->currentIndex().row());
	cubeList->selectionModel()->clear();
	cubeWorldModel->removeCube(cube);

	// Update view state
	removeCube->setEnabled(false);
}

void DesignView::saveModelClicked()
{

}

void DesignView::loadModelClicked()
{

}

void DesignView::clearModelClicked()
{

}

void DesignView::cubeListSelectionChange()
{
	removeCube->setEnabled(cubeList->currentItem() != Q_NULLPTR);
}

//// Layer 1 Row 1
//cubeWorldModel->insertCube(glm::vec3(1.8, -2, -5));
//cubeWorldModel->insertCube(glm::vec3(0.6, -2, -5));
//cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -5));
//cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -5));

//// Layer 1 Row 2
//cubeWorldModel->insertCube(glm::vec3(1.8, -2, -6.2));
//cubeWorldModel->insertCube(glm::vec3(0.6, -2, -6.2));
//cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -6.2));
//cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -6.2));

//// Layer 1 Row 3
//cubeWorldModel->insertCube(glm::vec3(1.8, -2, -7.4));
//cubeWorldModel->insertCube(glm::vec3(0.6, -2, -7.4));
//cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -7.4));
//cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -7.4));

//// Layer 1 Row 4
//cubeWorldModel->insertCube(glm::vec3(1.8, -2, -8.6));
//cubeWorldModel->insertCube(glm::vec3(0.6, -2, -8.6));
//cubeWorldModel->insertCube(glm::vec3(-0.6, -2, -8.6));
//cubeWorldModel->insertCube(glm::vec3(-1.8, -2, -8.6));

//// Layer 2 Row 1
//cubeWorldModel->insertCube(glm::vec3(1.2, -1, -5.6));
//cubeWorldModel->insertCube(glm::vec3(0.0, -1, -5.6));
//cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -5.6));

//// Layer 2 Row 2
//cubeWorldModel->insertCube(glm::vec3(1.2, -1, -6.8));
//cubeWorldModel->insertCube(glm::vec3(0.0, -1, -6.8));
//cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -6.8));

//// Layer 2 Row 3
//cubeWorldModel->insertCube(glm::vec3(1.2, -1, -8.0));
//cubeWorldModel->insertCube(glm::vec3(0.0, -1, -8.0));
//cubeWorldModel->insertCube(glm::vec3(-1.2, -1, -8.0));

//// Layer 3 Row 1
//cubeWorldModel->insertCube(glm::vec3(0.6, 0, -6.2));
//cubeWorldModel->insertCube(glm::vec3(-0.6, 0, -6.2));

//// Layer 3 Row 2
//cubeWorldModel->insertCube(glm::vec3(0.6, 0, -7.4));
//cubeWorldModel->insertCube(glm::vec3(-0.6, 0, -7.4));

//// Layer 4 Row 1
//cubeWorldModel->insertCube(glm::vec3(0, 1, -6.8));