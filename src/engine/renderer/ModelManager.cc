#include "src/pch.h"
#include <iostream>
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "Animation.h"
#include "MD5Animation.h"
#include "MD5Model.h"
#include "Model.h"
#include "ModelManager.h"

namespace energonsoftware {

Logger& ModelManager::logger(Logger::instance("gled.engine.renderer.ModelManager"));

void ModelManager::destroy(ModelManager* const manager, MemoryAllocator* const allocator)
{
    manager->~ModelManager();
    operator delete(manager, *allocator);
}

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager() throw()
{
}

bool ModelManager::load_animation(const boost::filesystem::path& path, const std::string& model, const std::string& name)
{
    // animations go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    boost::shared_ptr<Animation> animation;
    if(boost::filesystem::exists(animation_dir() / path / (name + MD5Animation::extension()))) {
        animation.reset(new(16, allocator) MD5Animation(name), boost::bind(&MD5Animation::destroy, _1, &allocator));
    } else if(boost::filesystem::exists(animation_dir() / path / (name + Animation::extension()))) {
        animation.reset(new(16, allocator) Animation(name), boost::bind(&Animation::destroy, _1, &allocator));
    }

    if(!animation) {
        LOG_ERROR("Could not find animation '" << name << "'!\n");
        return false;
    }

    if(!animation->load(path)) {
        LOG_ERROR("Error loading animation '" << name << "'!\n");
        return false;
    }
    animation->build_skeletons();

    _animations[model + "_" + name] = animation;
    return true;
}

boost::shared_ptr<Animation> ModelManager::animation(const std::string& model, const std::string& name) const
{
    try {
        return _animations.at(model + "_" + name);
    } catch(const std::out_of_range&) {
        LOG_WARNING("No such animation '" << name << "' for model '" << model << "'!\n");
        return boost::shared_ptr<Animation>();
    }
}

bool ModelManager::load_model(const boost::filesystem::path& path, const std::string& name)
{
    // models go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    boost::shared_ptr<Model> model;
    if(boost::filesystem::exists(model_dir() / path / (name + MD5Model::extension()))) {
        model.reset(new(16, allocator) MD5Model(name), boost::bind(&MD5Model::destroy, _1, &allocator));
    } else if(boost::filesystem::exists(model_dir() / path / (name + Model::extension()))) {
        model.reset(new(16, allocator) Model(name), boost::bind(&Model::destroy, _1, &allocator));
    }

    if(!model) {
        LOG_ERROR("Could not find model '" << name << "'!\n");
        return false;
    }

    if(!model->load(path)) {
        LOG_ERROR("Error loading model '" << name << "'!\n");
        return false;
    }

    _models[name] = model;
    return true;
}

boost::shared_ptr<Model> ModelManager::model(const std::string& name) const
{
    try {
        return _models.at(name);
    } catch(const std::out_of_range&) {
        return boost::shared_ptr<Model>();
    }
}

}
