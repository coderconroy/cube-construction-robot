#include "DesignView.h"
#include <glm/glm.hpp>;

DesignView::DesignView(QWidget* parent): QWidget(parent) 
{
	// Initialize cube world model
	cubeWorldModel = new CubeWorldModel(64, 10, this);
	connect(cubeWorldModel, &CubeWorldModel::log, this, &DesignView::log); // Propagate log signal

	// Initialize 3D shape design controls
	insertCube = new QPushButton("Insert Cube");
	removeCube = new QPushButton("Remove Cube");
	saveModel = new QPushButton("Save Model");
	loadModel = new QPushButton("Load Model");
	clearModel = new QPushButton("Clear Model");
	cubeList = new QListWidget();

	cubeList->setMaximumWidth(200);

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

	connect(shapeView, &OpenGLView::cubePositionUpdateRequested, cubeWorldModel, &CubeWorldModel::updateSelectedCubePosition);
	connect(shapeView, &OpenGLView::cubeOrientationUpdateRequested, cubeWorldModel, &CubeWorldModel::updateSelectedCubeOrientation);
	connect(shapeView, &OpenGLView::log, this, &DesignView::log); // Propagate log signal

	// Initialize controls layout
	controlsLayout = new QVBoxLayout();

	controlsLayout->addWidget(loadModel);
	controlsLayout->addWidget(saveModel);
	controlsLayout->addWidget(clearModel);
	controlsLayout->addWidget(insertCube);
	controlsLayout->addWidget(removeCube);
	controlsLayout->addWidget(cubeList);

	updateControlState();

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
	const Cube* cube = cubeWorldModel->insertCube();

	// Add cube to cube list widget
	QString cubeDescription = "Cube " + QString::number(cube->getCubeID());
	cubeMap.insert(cubeDescription, cube);
	cubeList->addItem(cubeDescription);

	// Select newly added cube in the cube list widget
	cubeList->setCurrentRow(cubeList->count() - 1);

	// Update view state
	updateControlState();
}

void DesignView::removeCubeClicked()
{
	// Get selected cube from cube list
	QString cubeDescription = cubeList->currentItem()->text();
	const Cube* cube = cubeMap.value(cubeDescription);

	// Remove selected cube from cube world model, cube list and cube map
	cubeMap.remove(cubeDescription);
	cubeWorldModel->removeCube(cube);
	cubeList->takeItem(cubeList->currentIndex().row());

	// Update view state
	updateControlState();
}

void DesignView::saveModelClicked()
{
	// Update view state
	updateControlState();
}

void DesignView::loadModelClicked()
{
	// Update view state
	updateControlState();
}

void DesignView::clearModelClicked()
{
	// Remove all cubes from cube world model, cube list and cube map
	cubeMap.clear();
	cubeList->clear();
	cubeWorldModel->clearCubes();

	// Update view state
	updateControlState();
}

void DesignView::cubeListSelectionChange()
{
	// Enable remove button only if a cube is selected
	removeCube->setEnabled(cubeList->currentItem() != Q_NULLPTR);

	// Select cube in cube world model
	if (cubeList->currentItem() != Q_NULLPTR)
	{
		// Get selected cube from cube list
		QString cubeDescription = cubeList->currentItem()->text();
		const Cube* cube = cubeMap.value(cubeDescription);
		cubeWorldModel->selectCube(cube);
	}
}

void DesignView::updateControlState()
{
	// Set enable state of control buttons
	if (cubeWorldModel->getCubeCount() > 0)
	{
		saveModel->setEnabled(true);
		loadModel->setEnabled(false);
		clearModel->setEnabled(true);
	}
	else
	{
		saveModel->setEnabled(false);
		loadModel->setEnabled(true);
		clearModel->setEnabled(false);
	}

	// Enable remove button only if a cube is selected
	removeCube->setEnabled(cubeList->currentItem() != Q_NULLPTR);
}