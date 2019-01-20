TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L/usr/local/lib -lSDL2 -lGL -lglut -lGLEW -lpthread

SOURCES += \
        main.cpp \
    Systems/PhysicsSystem.cpp \
    Systems/RenderingSystem.cpp \
    Systems/CollisionSystem.cpp

HEADERS += \
    Components/GraphicComponent.h \
    Components/HealthComponent.h \
    Components/LightComponent.h \
    Components/MagneticComponent.h \
    Components/PhysicsComponent.h \
    Systems/PhysicsSystem.h \
    Systems/RenderingSystem.h \
    Events/Collision.h \
    Events/EntityMoved.h \
    ECS.h \
    Engine.h \
    Systems/CollisionSystem.h
