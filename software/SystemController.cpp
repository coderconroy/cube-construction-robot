#include "SystemController.h"

SystemController::SystemController(QWidget *parent): QWidget(parent)
{
	// Initialize Home view
	homeView = new HomeView();

	// Initialize the base layout
	baseLayout = new QStackedLayout();

	// Add Home view to base layout
	baseLayout->addWidget(homeView);

	// Add base layout to window
	setLayout(baseLayout);
}
