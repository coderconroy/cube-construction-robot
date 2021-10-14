#include "Logger.h"

Logger::Logger(QWidget* parent): QWidget(parent)
{
	QHBoxLayout* baseLayout; /*! Layout for message display and supporting widgets */
	QVBoxLayout* controlLayout; /*! Layout for the message display controls */

	// Initialize display
	display = new QTextEdit();
	display->setReadOnly(true);

	// Initialize controls
	hideLog = new QPushButton("Hide");
	clear = new QPushButton("Clear");
	showInfo = new QCheckBox("Info");
	showWarnings = new QCheckBox("Warnings");
	showErrors = new QCheckBox("Errors");

	showInfo->setChecked(true);
	showWarnings->setChecked(true);
	showErrors->setChecked(true);

	// Connect control signals
	connect(hideLog, &QPushButton::clicked, this, &Logger::hideLogClicked);
	connect(clear, &QPushButton::clicked, this, &Logger::clearLog);
	connect(showInfo, &QCheckBox::clicked, this, &Logger::updateDisplay);
	connect(showWarnings, &QCheckBox::clicked, this, &Logger::updateDisplay);
	connect(showErrors, &QCheckBox::clicked, this, &Logger::updateDisplay);

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
	messageLog.append(message);
	updateDisplay();
}

void Logger::clearLog()
{
	messageLog.clear();
	display->clear();
}

void Logger::hideLogClicked()
{
	emit hideRequested();
}

void Logger::updateDisplay()
{
	// Clear message display
	display->clear();

	// Display messages in message log
	QList<Message>::iterator iter;
	for (iter = messageLog.begin(); iter != messageLog.end(); ++iter)
	{
		// Display message type
		switch (iter->type)
		{
		case MessageType::INFO_LOG:
			// Do not display message if info messages are disabled
			if (!showInfo->isChecked())
				continue;

			// Display message as info message
			display->insertHtml("<span style=\"font-family:Consolas;font-size:13px\"><pre><b>INFO</b>   \t</pre></span>");
			break;

		case MessageType::WARNING_LOG:
			// Do not display message if warning messages are disabled
			if (!showWarnings->isChecked())
				continue;

			// Display message as warning message
			display->insertHtml("<span style=\"font-family:Consolas;font-size:13px\"><pre><b>WARNING</b>\t</pre></span>");
			break;

		case MessageType::ERROR_LOG:
			// Do not display message if error messages are disabled
			if (!showErrors->isChecked())
				continue;

			// Display message as error message
			display->insertHtml("<span style=\"font-family:Consolas;font-size:13px\"><pre><b>ERROR</b>  \t</pre></span>");
			break;
		}

		// Display message source and content
		display->insertHtml("<em>" + iter->source + "</em> - " + iter->content + "<br>");
	}
}