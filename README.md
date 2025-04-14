
## HyperEngine

Vulkan-based game engine / renderer, with capability for VR and some modding.

C++ DLL/so scripting, with support for multi-platform (windows and linux.)
Dynamically-loaded game "modules" allows support for some modding or extra modularity for developers.

ImGui-based editor application, for creating environments and prefabs.
High editor and engine customisability via C++ modules.

## Renderer Features

- [ ] GPU-Driven geometry pipeline with culling
- [ ] Asynchronous GPU asset streaming & memory management
- [ ] VR rendering with multiview and quadview support

Extra Goals:
- [ ] Some advanced lighting (GI, shadowmapping, postprocessing)
- [ ] Streamed and Runtime Virtual Textures

## Used Libraries

- [Daxa](https://github.com/Ipotrick/Daxa) ([My own fork](https://github.com/Willoguns01/Daxa))
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [JoltPhysics](https://github.com/jrouwe/JoltPhysics)
- [EnTT](https://github.com/skypjack/entt)
- [EnkiTS](https://github.com/dougbinks/enkiTS)
- [Spdlog](https://github.com/gabime/spdlog)
