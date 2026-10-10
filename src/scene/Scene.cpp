#include "Scene.hpp"
#include "Room.hpp"
#include <numbers>
#include <vector>

using std::vector;

namespace
{
    vector<Wall> createHouseRoom()
    {
        const double width = 24.0;
        const double depth = 22.0;
        const double height = 6.0;
        const double doorHeight = 4.0;

        vector<Wall> walls = createBoxRoom(width, depth, height);

        // add a partition at fixed Y, leaving a doorway below its lintel.
        auto addDoorway = [&](double minX, double maxX, double y,
                              double doorMinX, double normalY, double doorWidth = 2.0)
        {
            const double doorMaxX = doorMinX + doorWidth;
            const Plane plane({minX, y, 0}, {0, normalY, 0});

            walls.push_back(Wall(plane, Vec3(doorMinX, y, height), Vec3(minX, y, 0)));
            walls.push_back(Wall(plane, Vec3(maxX, y, height), Vec3(doorMaxX, y, 0)));
            walls.push_back(Wall(plane, Vec3(doorMaxX, y, height), Vec3(doorMinX, y, doorHeight)));
        };

        addDoorway(0.0, 11.0, 12.0, 3.0, 1.0, 4.0);
        addDoorway(11.0, 16.0, 12.0, 12.75, 1.0, 1.5);
        addDoorway(16.0, 24.0, 12.0, 18.0, 1.0, 3.0);
        walls.push_back(Wall(Plane({11, 12, 0}, {-1, 0, 0}),
                             Vec3(11, depth, height), Vec3(11, 12, 0)));
        walls.push_back(Wall(Plane({16, 12, 0}, {-1, 0, 0}),
                             Vec3(16, depth, height), Vec3(16, 12, 0)));

        // solid pillar. acts as an obstacle
        walls.push_back(Wall(Plane({15, 5, 0}, {-1, 0, 0}),
                             Vec3(15, 8, height), Vec3(15, 5, 0)));
        walls.push_back(Wall(Plane({18, 5, 0}, {1, 0, 0}),
                             Vec3(18, 8, height), Vec3(18, 5, 0)));
        walls.push_back(Wall(Plane({15, 5, 0}, {0, -1, 0}),
                             Vec3(18, 5, height), Vec3(15, 5, 0)));
        walls.push_back(Wall(Plane({15, 8, 0}, {0, 1, 0}),
                             Vec3(18, 8, height), Vec3(15, 8, 0)));

        return walls;
    }
}

Scene createBoxScene()
{
    Scene scene{};

    scene.walls = createBoxRoom(10.0, 8.0, 6.0);
    scene.sourcePosition = Vec3(5.0, 4.0, 3.0);
    scene.receiver.position = Vec3(8.0, 6.0, 2.0);
    scene.receiver.radius = 1.5;

    scene.cameraTarget = Vec3(5.0, 4.0, 3.0);
    scene.cameraDistance = 24;
    scene.cameraYaw = 0.0f;
    scene.cameraPitch = 52.0f * std::numbers::pi_v<float> / 180.0f;

    return scene;
}
Scene createLShapedScene()
{
    Scene scene{};

    scene.walls = createLShapedRoom(16.0, 16.0, 6.0, 4.0);
    scene.sourcePosition = Vec3(7.0, 2.0, 3.0);
    scene.receiver.position = Vec3(2.0, 6.0, 3.0);
    scene.receiver.radius = 1.5;

    scene.cameraTarget = Vec3(8.0, 8.0, 3.0);
    scene.cameraDistance = 34;
    scene.cameraYaw = 0.0f;
    scene.cameraPitch = 52.0f * std::numbers::pi_v<float> / 180.0f;

    return scene;
}

Scene createHouseScene()
{
    Scene scene{};

    scene.walls = createHouseRoom();
    scene.sourcePosition = Vec3(8.0, 4.5, 3.0);
    scene.receiver.position = Vec3(5.0, 17.0, 3.0);
    scene.receiver.radius = 1.5;

    scene.cameraTarget = Vec3(12.0, 11.0, 3.0);
    scene.cameraDistance = 38.0f;
    scene.cameraYaw = 0.0f;
    scene.cameraPitch = 52.0f * std::numbers::pi_v<float> / 180.0f;

    return scene;
}
