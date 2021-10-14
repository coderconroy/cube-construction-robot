#include "SystemController.h"

SystemController::SystemController(QWidget *parent): QWidget(parent)
{
	// Initialize views
	homeView = new HomeView();
	designView = new DesignView();
	constructionView = new ConstructionView();

	// Initialize navigation widgets
	homeViewLink = new QPushButton("Home");
	designViewLink = new QPushButton("3D Shape Design");
	constructionViewLink = new QPushButton("Construction");

	homeViewLink->setMinimumHeight(40);
	designViewLink->setMinimumHeight(40);
	constructionViewLink->setMinimumHeight(40);

	// Connect navigation button signals and slots
	connect(homeViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(designViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(constructionViewLink, &QPushButton::clicked, this, &SystemController::setView);

	// Initialize navigation bar widgets
	navigationLayout = new QHBoxLayout();
	navigationLayout->addWidget(homeViewLink);
	navigationLayout->addWidget(designViewLink);
	navigationLayout->addWidget(constructionViewLink);

	// Initialize primary view container
	viewLayout = new QStackedLayout();
	viewLayout->addWidget(homeView);
	viewLayout->addWidget(designView);
	viewLayout->addWidget(constructionView);
	viewLayout->setCurrentWidget(homeView);

	// Initialize the base layout
	baseLayout = new QVBoxLayout();

	// Add Home view to base layout
	baseLayout->addLayout(navigationLayout);
	baseLayout->addLayout(viewLayout);

	// Add base layout to window
	setLayout(baseLayout);
}

void SystemController::setView()
{
	// Get signal source
	QObject* source = sender();

	// Set view based on source button
	if (source == homeViewLink)
	{
		viewLayout->setCurrentWidget(homeView);
	}
	else if (source == designViewLink)
	{
		viewLayout->setCurrentWidget(designView);
	}
	else if (source == constructionViewLink)
	{
		viewLayout->setCurrentWidget(constructionView);
	}
}