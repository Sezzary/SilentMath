#pragma once

namespace Silent::Renderer
{
    /** @brief GPU texture. */
    class Texture
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

        /** @brief Constructs an uninitialized default `Texture`. */
        Texture() = default;

        /** @brief Constructs a `Texture` from a texture image and uploads it to the GPU.
         *
         * @param device GPU device.
         * @param copyPass Copy pass.
         * @param pixels Texture image pixels.
         * @param res Texture image resolution.
         * @param name Texture name.
         */
        Texture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i& res, const std::string& name);

        /** @brief Gracefully destroys the `Texture` and frees GPU resources. */
        ~Texture();

        // ==========
        // Utilities
        // ==========

        void Initialize(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name = {});

        /** @brief Updates a specified region of pixels in the texture.
         *
         * @param copyPass Copy pass.
         * @param pixels Updated texture image pixels.
         * @param region Update region position.
         * @param size Update region size.
         */
        void Update(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i& region, const Vector2i& size);

        /** @brief Binds the texture for use on the active fragment shader.
         *
         * @param renderPass Render pass.
         * @param sampler Texture sampler.
         */
        void Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler);
    };

    /** @brief GPU texture manager. */
    class TextureManager
    {
    private:
        
        SDL_GPUDevice*                           _device   = nullptr;
        std::unordered_map<std::string, Texture> _textures = {}; /** Key = name, value = texture. */

    public:
        TextureManager() = default;

        ~TextureManager();

        Texture* Get(const std::string& name);

        void Initialize(SDL_GPUDevice& device);

        /** @brief Loads a texture from a texture image.
         *
         * @param copyPass Copy pass.
         * @param pixels Texture image pixels.
         * @param res Texture image resolution.
         * @param Texture name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name = {});

        /** @brief Loads a texture from a TIM asset.
         *
         * @param copyPass Copy pass.
         * @param assetIdx TIM asset index.
         */
        void Load(SDL_GPUCopyPass& copyPass, int assetIdx);
    };
}
