#pragma once

namespace Silent::Renderer
{
    class ShaderProgram
    {
    private:
        // Fields

        uint        _id   = 0;
        std::string _name = {};

    public:
        // Constructors

        ShaderProgram(const std::string& name);

        // Getters

        uint               GetId() const;
        const std::string& GetName() const;

        // Setters

        void SetBool(const std::string& uniName, bool val) const;
        void SetInt(const std::string& uniName, int val) const;
        void SetUint(const std::string& uniName, uint val) const;
        void SetFloat(const std::string& uniName, float val) const;
        void SetVec2(const std::string& uniName, const Vector2& val) const;
        void SetVec3(const std::string& uniName, const Vector3& val) const;
        void SetVec4(const std::string& uniName, const Vector4& val) const;
        void SetMatrix(const std::string& uniName, const Matrix& val) const;
        void SetIntArray(const std::string& uniName, const std::span<int>& val) const;
        void SetUintArray(const std::string& uniName, const std::span<uint>& val) const;
        void SetFloatArray(const std::string& uniName, const std::span<float>& val) const;
        void SetVec2Array(const std::string& uniName, const std::span<Vector2>& val) const;
        void SetVec3Array(const std::string& uniName, const std::span<Vector3>& val) const;
        void SetVec4Array(const std::string& uniName, const std::span<Vector4>& val) const;
        void SetMatrixArray(const std::string& uniName, const std::span<Matrix>& val) const;

        // Utilities

        void Activate();
        void Delete();

    private:
        // Helpers

        std::string GetFileContents(const std::string& filename);
        void        LogShaderError(uint shaderId, const std::string& type);
        void        LogProgramError(uint programId);
    };
}
