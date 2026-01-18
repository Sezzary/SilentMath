#pragma once

#include "Renderer/Common/Texture.h"

namespace Silent::Renderer
{
    /** @brief GPU texture. */
    class SdlGpuTexture : public ITexture
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
        /** @brief Constructs an uninitialized default `Texture`. */
        SdlGpuTexture() = default;

        /** @brief Constructs a `Texture` and uploads data to the GPU.
         *
         * @param device GPU device.
         * @param copyPass Copy pass.
         * @param pixels Texture image pixels.
         * @param res Texture image resolution.
         * @param name Texture image name.
         */
        SdlGpuTexture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, std::span<const byte> pixels, const Vector2i res, const std::string& name);

        /** @brief Gracefully destroys the `Texture` and frees GPU resources. */
        ~SdlGpuTexture();

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
         */
        void Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler);
    };

    /** @brief GPU texture cache manager. */
    class SdlGpuTextureManager : public TextureManagerBase
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

        /** @brief Constructs an `SdlGpuTextureManager` with a device reference.
         *
         * @param device GPU device.
         */
        SdlGpuTextureManager(SDL_GPUDevice& device);

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

        /** @brief Loads a texture from a TIM asset.
         *
         * @param copyPass Copy pass.
         * @param assetIdx TIM asset name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::string& assetName);

        SdlGpuTexture* operator[](const std::string& name);
    };
}
