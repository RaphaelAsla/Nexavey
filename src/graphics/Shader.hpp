#include <glad/gl.h>

#include <string>

namespace nex {
    // Code from learnopengl.com (https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h)
    class Shader {
      public:
        unsigned int ID;
        Shader() = default;
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char* vertexPath, const char* fragmentPath);
        // activate the shader
        // ------------------------------------------------------------------------
        void Use();
        // utility uniform functions
        // ------------------------------------------------------------------------
        void SetBool(const std::string& name, bool value) const;
        // ------------------------------------------------------------------------
        void SetInt(const std::string& name, int value) const;
        // ------------------------------------------------------------------------
        void SetFloat(const std::string& name, float value) const;
        // ------------------------------------------------------------------------
        void Delete();

      private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void CheckCompileErrors(unsigned int shader, std::string type);
    };
}  // namespace nex
