#include "SystemController.h"

SystemController::SystemController(QWidget *parent): QWidget(parent)
{
	// Initialize robot interface
	robot = new Robot(this);

	// Initialize camera
	camera = new cv::VideoCapture(0);
	if (!camera->isOpened())
		messageLog->log(Message(MessageType::ERROR_LOG, "System Controller", "No camera found"));

	// Initialize views
	homeView = new HomeView();
	designView = new DesignView();
	constructionView = new ConstructionView();

	homeView->setRobot(robot);
	homeView->setCamera(camera);
	constructionView->setRobot(robot);
	constructionView->setCamera(camera);

	connect(homeView, &HomeView::robotConnected, this, &SystemController::robotConnected);

	// Initialize navigation widgets
	homeViewLink = new QPushButton("Home");
	designViewLink = new QPushButton("3D Shape Design");
	constructionViewLink = new QPushButton("Construction");
	showMessageLog = new QPushButton();

	constructionViewLink->setEnabled(false);

	showMessageLog->setIcon(QIcon("res/log-icon.png"));
	showMessageLog->setIconSize(QSize(32, 32));

	homeViewLink->setMinimumHeight(40);
	designViewLink->setMinimumHeight(40);
	constructionViewLink->setMinimumHeight(40);
	showMessageLog->setFixedSize(40, 40);

	// Connect navigation button signals and slots
	connect(homeViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(designViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(constructionViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(showMessageLog, &QPushButton::clicked, this, &SystemController::showMessageLogClick);

	// Initialize navigation bar widgets
	navigationLayout = new QHBoxLayout();
	navigationLayout->addWidget(homeViewLink);
	navigationLayout->addWidget(designViewLink);
	navigationLayout->addWidget(constructionViewLink);
	navigationLayout->addWidget(showMessageLog);
	showMessageLog->hide();

	// Initialize message logger
	messageLog = new Logger();
	connect(messageLog, &Logger::hideRequested, this, &SystemController::hideMessageLog);

	connect(homeView, &HomeView::log, messageLog, &Logger::log);
	connect(designView, &DesignView::log, messageLog, &Logger::log);
	connect(constructionView, &ConstructionView::log, messageLog, &Logger::log);

	// Initialize primary view container
	viewLayout = new QStackedLayout();
	viewLayout->addWidget(homeView);
	viewLayout->addWidget(designView);
	viewLayout->addWidget(constructionView);
	viewLayout->setCurrentWidget(homeView);
	homeView->showView();

	// Initialize the base layout
	baseLayout = new QVBoxLayout();

	// Add Home view to base layout
	baseLayout->addLayout(navigationLayout);
	baseLayout->addLayout(viewLayout);
	baseLayout->addWidget(messageLog);

	// Add base layout to window
	setLayout(baseLayout);
}

SystemController::~SystemController()
{
	delete camera;
}

void SystemController::setView()
{
	// Get signal source
	QObject* source = sender();

	// Set all views to hide state
	homeView->hideView();
	designView->hideView();
	constructionView->hideView();

	// Set view based on source button
	if (source == homeViewLink)
	{
		homeView->showView();
		viewLayout->setCurrentWidget(homeView);
	}
	else if (source == designViewLink)
	{
		designView->showView();
		viewLayout->setCurrentWidget(designView);
	}
	else if (source == constructionViewLink)
	{
		constructionView->showView();
		viewLayout->setCurrentWidget(constructionView);
	}
}

void SystemController::hideMessageLog()
{
	messageLog->hide();
	showMessageLog->show();
}

void SystemController::showMessageLogClick()
{
	messageLog->show();
	showMessageLog->hide();
}

void SystemController::robotConnected()
{
	constructionViewLink->setEnabled(true);
}