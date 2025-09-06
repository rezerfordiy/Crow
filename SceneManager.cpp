#include "SceneManager.h"
#include "MySceneManager.h"

std::unique_ptr<SceneManager> SceneManager::create(MyScene* scene) {
    return std::make_unique<MySceneManager>(scene);
}
