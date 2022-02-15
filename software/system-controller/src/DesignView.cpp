#include "DesignView.h"
#include <glm/glm.hpp>;
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QFileDialog>

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
	cubeXPosition = new QLabel("X Position = 0 mm");
	cubeYPosition = new QLabel("Y Position = 0 mm");
	cubeZPosition = new QLabel("Z Position = Layer 0");
	cubeRotation = new QLabel("Rotation = 0 degrees");
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
	shapeView = new OpenGLView();
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
	//controlsLayout->addWidget(cubeXPosition);
	//controlsLayout->addWidget(cubeYPosition);
	//controlsLayout->addWidget(cubeZPosition);
	//controlsLayout->addWidget(cubeRotation);
	controlsLayout->addWidget(cubeList);

	updateControlState();

	// Initialize design view layout
	baseLayout = new QHBoxLayout();
	baseLayout->addLayout(controlsLayout);
	baseLayout->addWidget(shapeView);

	setLayout(baseLayout);

	// Initialize OpenGL shape view timer
	openGLTimer = new QTimer(this);
	connect(openGLTimer, &QTimer::timeout, this, &DesignView::updateShapeView);
}

void DesignView::showView()
{
	openGLTimer->start(20); // Update OpenGL render every 20ms
}

void DesignView::hideView()
{
	openGLTimer->stop(); // Do not refresh OpenGL render when the design view is hidden
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
	// Select JSON cube world model file to save to
	QString fileName = QFileDialog::getSaveFileName(this, "Save Cube World Model", "", "Cube Model Files (*.cubeworld)");

	// Verify a file was selected
	if (fileName.isNull())
		return;

	// Convert cube world model to JSON object
	QJsonObject jsonCubeWorldModel;
	cubeWorldModel->write(jsonCubeWorldModel);

	// Write JSON cube world model to file
	QJsonDocument document;
	document.setObject(jsonCubeWorldModel);
	QByteArray jsonBytes = document.toJson(QJsonDocument::Indented);
	QFile jsonFile(fileName);
	if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		QTextStream textStream(&jsonFile);
		textStream.setEncoding(QStringConverter::Utf8);
		textStream << jsonBytes;
		jsonFile.close();
		emit log(Message(MessageType::INFO_LOG, "Design View", "Cube world model successfully saved to file"));
	}
	else
	{
		emit log(Message(MessageType::ERROR_LOG, "Design View", "Failed to open file to write cube world JSON model to"));
	}

	// Update view state
	updateControlState();
}

void DesignView::loadModelClicked()
{
	// Select JSON cube world model file from file system
	QString fileName = QFileDialog::getOpenFileName(this, "Open Cube World Model", "", "Cube Model Files (*.cubeworld)");

	// Verify a file was selected
	if (fileName.isNull())
		return;

	// Read JSON cube world model from file
	QFile jsonFile(fileName);
	if (jsonFile.open(QIODevice::ReadOnly))
	{
		// Read file into byte array
		QByteArray jsonBytes = jsonFile.readAll();
		jsonFile.close();

		// Parse JSON document from byte array
		QJsonParseError jsonError;
		QJsonDocument document = QJsonDocument::fromJson(jsonBytes, &jsonError);
		if (jsonError.error != QJsonParseError::NoError)
		{
			QString errorMessage = QString("Failed to read from JSON cube world model file: ") + jsonError.errorString();
			emit log(Message(MessageType::ERROR_LOG, "Design View", errorMessage));
			return;
		}

		// Initialize cube world model with JSON object
		cubeWorldModel->read(document.object());
		emit log(Message(MessageType::INFO_LOG, "Design View", "Cube world model successfully loaded from file"));
	}

	// Populate cube list widget with loaded cubes
	const QList<Cube*>* cubes = cubeWorldModel->getCubes();
	for (int i = 0; i < cubes->size(); ++i)
	{
		// Add cube to cube list widget
		QString cubeDescription = "Cube " + QString::number(cubes->value(i)->getCubeID());
		cubeMap.insert(cubeDescription, cubes->value(i));
		cubeList->addItem(cubeDescription);
	}

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