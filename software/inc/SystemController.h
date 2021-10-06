#include <QWidget>
#include <QStackedLayout>
#include "HomeView.h"
#include "DesignView.h"

class SystemController: public QWidget
{
    Q_OBJECT

public:
    SystemController(QWidget *parent = Q_NULLPTR);

private:
    QStackedLayout* baseLayout;
    HomeView* homeView;
    DesignView* designView;
};
