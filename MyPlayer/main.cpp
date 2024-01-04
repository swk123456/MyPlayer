
#include <iostream>
#include <QApplication>
#include "MyPlayer.h"
#include "PlayerControl.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFile styleSheetFile("./Filmovio.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    app.setStyleSheet(styleSheet);

    MyPlayer player;
    player.show();

    return app.exec();
}