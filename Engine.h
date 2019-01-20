#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

#include "ECS.h"

#define MS_PER_UPDATE 17

class Engine
{
public:
    Engine() : m_running(false)
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
            exit(1);
        }

        m_window = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if(!m_window) {
            std::cout << "Failed to create a window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        // Create an OpenGL context (so we can use OpenGL functions)
        m_context = SDL_GL_CreateContext(m_window);
        if(!m_context) {
            // we'll print out an error message and exit
            std::cerr << "Failed to create a valid OpenGL context\n";
            exit(2);
        }

        if(!init()) {
            std::cout << "Failed to initialize engine" << std::endl;
            exit(3);
        }
    }

    virtual ~Engine()
    {
        // Destroy the context
        SDL_GL_DeleteContext(m_context);

        // Close and destroy the window
        SDL_DestroyWindow(m_window);

        // Clean up
        SDL_Quit();

        ECS::cleanUp();

        cleanUp();
    }

    int run()
    {
        m_running = true;

        while(m_running) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            processEvents();
            updateSystems(1.0f/60.0f);
            draw();

            // Swap OpenGL buffers
            SDL_GL_SwapWindow(m_window);
        }
        return 0;
    }

    SDL_Window *window() { return m_window; }
    SDL_GLContext &context() { return m_context; }

protected:
    virtual bool init() { return true; }
    virtual void handleEvent(SDL_Event &event) {}
    virtual void draw() {}
    virtual void cleanUp() {}

private:
    SDL_Window *m_window;
    SDL_GLContext m_context;
    bool m_running;

    void processEvents()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            m_running = event.type != SDL_QUIT;

            // handle window resize here
            if(event.type == SDL_WINDOWEVENT)
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    glViewport(0, 0, event.window.data1, event.window.data2);

            handleEvent(event);
        }
    }

    void updateSystems(float dt)
    {
        for(size_t i = 0; i < ECS::systems().size(); ++i) {
            if(ECS::systems()[i]) {
                ECS::systems()[i]->processEvents();
                ECS::systems()[i]->update(dt);
            }
        }

        for(size_t i = 0; i < ECS::systems().size(); ++i) {
            if(ECS::systems()[i])
                ECS::systems()[i]->processEvents();
        }
    }
};

#endif // ENGINE_H
