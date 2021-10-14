#include "Logger.h"

Logger::Logger(QWidget* parent): QWidget(parent)
{
	QHBoxLayout* baseLayout; /*! Layout for message display and supporting widgets */
	QVBoxLayout* controlLayout; /*! Layout for the message display controls */

	// Initialize message log
	messageLog = new QList<Message>();

	// Initialize display
	display = new QTextEdit();
	display->setReadOnly(true);

	// Initialize controls
	hideLog = new QPushButton("Hide");
	clear = new QPushButton("Clear");
	showInfo = new QCheckBox("Info");
	showWarnings = new QCheckBox("Warnings");
	showErrors = new QCheckBox("Errors");

	connect(hideLog, &QPushButton::clicked, this, &Logger::hideLogClicked);

	// Initialize control layout
	controlLayout = new QVBoxLayout();
	controlLayout->addWidget(hideLog);
	controlLayout->addWidget(clear);
	controlLayout->addWidget(showInfo);
	controlLayout->addWidget(showWarnings);
	controlLayout->addWidget(showErrors);
	controlLayout->addStretch();

	// Initialize base layout
	baseLayout = new QHBoxLayout();
	baseLayout->addWidget(display);
	baseLayout->addLayout(controlLayout);

	setLayout(baseLayout);
}

void Logger::log(const Message& message)
{

}

void Logger::clearLog()
{

}

void Logger::hideLogClicked()
{
	emit hideRequested();
}