#include "SystemController.h"

SystemController::SystemController(QWidget *parent): QWidget(parent)
{
	// Initialize views
	homeView = new HomeView();
	designView = new DesignView();

	// Initialize navigation widgets
	homeViewLink = new QPushButton("Home");
	designViewLink = new QPushButton("3D Shape Design");

	// Connect navigation button signals and slots
	connect(homeViewLink, &QPushButton::clicked, this, &SystemController::setView);
	connect(designViewLink, &QPushButton::clicked, this, &SystemController::setView);

	// Initialize navigation bar widgets
	navigationLayout = new QHBoxLayout();
	navigationLayout->addStretch();
	navigationLayout->addWidget(homeViewLink);
	navigationLayout->addWidget(designViewLink);
	navigationLayout->addStretch();

	// Initialize primary view container
	viewLayout = new QStackedLayout();
	viewLayout->addWidget(homeView);
	viewLayout->addWidget(designView);
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
}