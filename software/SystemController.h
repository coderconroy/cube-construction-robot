#include <QWidget>
#include <QStackedLayout>
#include "HomeView.h"

class SystemController : public QWidget
{
    Q_OBJECT

public:
    SystemController(QWidget *parent = Q_NULLPTR);

private:
    QStackedLayout* baseLayout;
    HomeView* homeView;
};
