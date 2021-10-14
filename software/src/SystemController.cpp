#include "SystemController.h"

SystemController::SystemController(QWidget *parent): QWidget(parent)
{
	// Initialize views
	homeView = new HomeView();
	designView = new DesignView();

	// Initialize the base layout
	baseLayout = new QStackedLayout();

	// Add Home view to base layout
	baseLayout->addWidget(homeView);
	//baseLayout->addWidget(designView);

	// Add base layout to window
	setLayout(baseLayout);
}
