
#include <iostream>
#include <QApplication>
#include "MyPlayer.h"
#include "PlayerControl.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MyPlayer player;
    player.show();

    return app.exec();
}