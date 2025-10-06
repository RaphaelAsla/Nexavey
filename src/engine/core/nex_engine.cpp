#include "nex_engine.hpp"

#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../graphics/nex_buffer.hpp"
#include "../graphics/nex_texture.hpp"
#include "../input/nex_input.hpp"
#include "../scene/nex_camera.hpp"
#include "../systems/point_light_system.hpp"
#include "../systems/shadowmap_system.hpp"
#include "../systems/simple_render_system.hpp"

namespace nex {
    NexEngine::NexEngine() {
        m_descriptor_pool =
            NexDescriptorPool::Builder(m_device).setMaxSets(NexSwapChain::max_frames_in_flight).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NexSwapChain::max_frames_in_flight).build();

        // build frame descriptor pools
        for (int i = 0; i < NexSwapChain::max_frames_in_flight; ++i) {
            m_frame_descriptor_pools.push_back(NexDescriptorPool::Builder(m_device)
                                                   .setMaxSets(1000)
                                                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100)
                                                   .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100)
                                                   .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100)
                                                   .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                                                   .build());
        }

        loadEntities();
    }

    NexEngine::~NexEngine() {}

    void NexEngine::run() {
        std::vector<std::unique_ptr<NexBuffer>> ubo_buffers(NexSwapChain::max_frames_in_flight);
        for (int i = 0; i < NexSwapChain::max_frames_in_flight; ++i) {
            ubo_buffers[i] = std::make_unique<NexBuffer>(m_device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            ubo_buffers[i]->map();
        }

        auto global_set_layout = NexDescriptorSetLayout::Builder(m_device).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

        std::vector<VkDescriptorSet> global_descriptor_sets(NexSwapChain::max_frames_in_flight);
        for (int i = 0; i < NexSwapChain::max_frames_in_flight; ++i) {
            auto buffer_info = ubo_buffers[i]->descriptorInfo();
            NexDescriptorWriter(*global_set_layout, *m_descriptor_pool).writeBuffer(0, &buffer_info).build(global_descriptor_sets[i]);
        }

        SimpleRenderSystem simple_render_system(m_device, m_renderer.getSwapChainRenderPass(), global_set_layout->getDescriptorSetLayout());
        PointLightSystem   point_light_system(m_device, m_renderer.getSwapChainRenderPass(), global_set_layout->getDescriptorSetLayout());
        ShadowSystem       shadow_system(m_device);

        NexCamera camera = {};

        auto viewer_object                        = NexEntity::create();
        viewer_object.m_transform.m_translation.z = -2.5f;
        Input camera_controller                   = {};

        auto current_time = std::chrono::high_resolution_clock::now();

        while (!m_window.shouldClose()) {
            glfwPollEvents();

            auto  new_time   = std::chrono::high_resolution_clock::now();
            float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
            current_time     = new_time;

            delta_time = std::min(delta_time, 0.1f);

            camera_controller.moveInPlaneXZ(m_window.getGLFWwindow(), delta_time, viewer_object);
            camera.setViewYXZ(viewer_object.m_transform.m_translation, viewer_object.m_transform.m_rotation);

            float aspect_ratio = m_renderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(50.0f), aspect_ratio, 0.1f, 100.0f);

            if (auto command_buffer = m_renderer.beginFrame()) {
                int frame_index = m_renderer.getFrameIndex();

                // Reset the frame descriptor pool for this frame
                m_frame_descriptor_pools[frame_index]->resetPool();

                NexFrameInfo frame_info{
                    frame_index, delta_time, command_buffer, camera, global_descriptor_sets[frame_index], *m_frame_descriptor_pools[frame_index], m_entities,
                };

                // update
                GlobalUbo ubo             = {};
                ubo.m_projection_matrix   = camera.getProjectionMatrix();
                ubo.m_view_matrix         = camera.getViewMatrix();
                ubo.m_inverse_view_matrix = camera.getInverseViewMatrix();
                point_light_system.update(frame_info, ubo);
                ubo_buffers[frame_index]->writeToBuffer(&ubo);
                ubo_buffers[frame_index]->flush();

                // render shadow map first
                shadow_system.renderShadowMap(frame_info);

                // render main scene
                m_renderer.beginSwapChainRenderPass(command_buffer);
                simple_render_system.renderEntities(frame_info, shadow_system.getShadowMapDescriptor(), shadow_system.getLightSpaceMatrix());
                point_light_system.render(frame_info);
                m_renderer.endSwapChainRenderPass(command_buffer);
                m_renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_device.device());
    }

    void NexEngine::loadEntities() {
        std::shared_ptr<NexMesh> nex_model    = NexMesh::createModelFromFile(m_device, "../models/viking_room.obj");
        auto                     viking_room  = NexEntity::create();
        viking_room.m_material_index          = 0;
        viking_room.m_model                   = nex_model;
        viking_room.m_transform.m_translation = {0.0f, 0.5f, 0.0f};
        viking_room.m_transform.m_rotation    = glm::vec3{glm::radians(90.0f), glm::radians(90.0f), 0.0f};
        viking_room.m_transform.m_scale       = glm::vec3{1.0f};
        viking_room.m_texture                 = NexTexture::createTextureFromFile(m_device, "../textures/viking_room.png");
        viking_room.m_texture->updateDescriptor();
        m_entities.emplace(viking_room.getId(), std::move(viking_room));

        nex_model                        = NexMesh::createModelFromFile(m_device, "../models/monkey.obj");
        auto monkey                      = NexEntity::create();
        monkey.m_material_index          = 1;
        monkey.m_model                   = nex_model;
        monkey.m_transform.m_translation = {1.5f, -1.0f, 0.0f};
        monkey.m_transform.m_rotation    = glm::vec3{glm::radians(180.0f), 0.0f, 0.0f};
        monkey.m_transform.m_scale       = glm::vec3{1.0f};
        m_entities.emplace(monkey.getId(), std::move(monkey));

        nex_model                       = NexMesh::createModelFromFile(m_device, "../models/quad.obj");
        auto floor                      = NexEntity::create();
        floor.m_material_index          = 1;
        floor.m_model                   = nex_model;
        floor.m_transform.m_translation = {0.0f, 0.5f, 0.0f};
        floor.m_transform.m_scale       = glm::vec3{3.0f};
        floor.m_texture                 = NexTexture::createTextureFromFile(m_device, "../textures/floor.png");
        floor.m_texture->updateDescriptor();
        m_entities.emplace(floor.getId(), std::move(floor));

        // auto light_left                      = NexEntity::makePointLight(0.3f, 0.1f, {1.0f, 0.84f, 0.4f});
        // light_left.m_transform.m_translation = {-0.56f, -0.10f, 0.25f};
        // light_left.m_transform.m_scale       = glm::vec3{0.0f};
        // m_entities.emplace(light_left.getId(), std::move(light_left));

        // auto light_right                      = NexEntity::makePointLight(0.3f, 0.1f, {1.0f, 0.84f, 0.4f});
        // light_right.m_transform.m_translation = {0.63f, -0.10f, 0.26f};
        // light_right.m_transform.m_scale       = glm::vec3{0.0f};
        // m_entities.emplace(light_right.getId(), std::move(light_right));
    }

}  // namespace nex
