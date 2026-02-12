#pragma once

#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Resources/TextureCache.h"

using namespace Silent::Assets;

namespace Silent::Renderer::SdlGpu
{
    /** @brief GPU texture. */
    class Texture : public TextureBase
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*  _device  = nullptr;
        SDL_GPUTexture* _texture = nullptr;

    public:
        // =============
        // Constructors
        // =============

        // @todo Deprecated.
        /** @brief Creates a default uninitialized instance. */
        Texture() = default;

        /** @brief Creates an instance and uploads data to the GPU.
         *
         * @param device GPU device.
         * @param copyPass Copy pass.
         * @param usageFlags Texture usage flags.
         * @param pixels Texture image pixels.
         * @param res Texture image resolution.
         * @param name Texture image name.
         */
        Texture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass,
                SDL_GPUTextureUsageFlags usageFlags, std::span<const byte> pixels, const Vector2i& res, const std::string& name);

        /** @brief Gracefully destroys the instance and frees GPU resources. */
        ~Texture();

        // ========
        // Getters
        // ========

        /** @brief Gets the handle to the underlying texture.
         *
         * @return Underlying texture handle.
         */
        SDL_GPUTexture* GetHandle();

        /** @brief Gets the texture's pixel resolution.
         *
         * @return Texture pixel resolution.
         */
        const Vector2i& GetResolution() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Updates a specified region of pixels in the texture.
         *
         * @param copyPass Copy pass.
         * @param pixels Updated texture image pixels.
         * @param region Start pixel position of the region to update (top-left corner).
         * @param size Pixel size of the region to update.
         */
        void Update(SDL_GPUCopyPass& copyPass, std::span<const byte> pixels, const Vector2i& region, const Vector2i& size);

        /** @brief Binds the texture for use on the active fragment shader.
         *
         * @param renderPass Render pass.
         * @param sampler Texture sampler.
         * @param slotIdx Shader sampler slot index.
         */
        void Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler, int slotIdx = 0);
    };

    /** @brief GPU texture cache. */
    class TextureCache : public TextureCacheBase
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice* _device = nullptr;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates an instance with a device reference.
         *
         * @param device GPU device.
         */
        TextureCache(SDL_GPUDevice& device);

        // ==========
        // Utilities
        // ==========

        /** @brief Loads a texture from a texture image. If the texture is already loaded, it will be overwritten with new data.
         *
         * @param copyPass Copy pass.
         * @param pixels Texture image pixels.
         * @param res Texture image resolution.
         * @param name Texture name.
         */
        void Load(SDL_GPUCopyPass& copyPass, std::span<const byte> pixels, const Vector2i res, const std::string& name);

        /** @brief Loads a texture from a streamable asset.
         *
         * @param copyPass Copy pass.
         * @param assetName Streamable asset name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::string& assetName);

        /** @brief Unloads */
        void Unload(const std::string& name);

        /** @brief Clears all textures in the cache. */
        void Clear();

        Texture* operator[](const std::string& name);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Loads a texture from a PNG asset.
         *
         * @param copyPass Copy pass.
         * @param asset PNG asset.
         */
        void LoadPng(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Loads a texture from a TIM asset.
         *
         * @param copyPass Copy pass.
         * @param asset TIM asset.
         */
        void LoadTim(SDL_GPUCopyPass& copyPass, const Asset& asset);
    };
}
