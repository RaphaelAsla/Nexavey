#include <glad/gl.h>

#include <string>

namespace nex {
    // Code from learnopengl.com (https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h)
    class Shader {
      public:
        unsigned int ID;
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char* vertexPath, const char* fragmentPath);
        // activate the shader
        // ------------------------------------------------------------------------
        void use();
        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string& name, bool value) const;
        // ------------------------------------------------------------------------
        void setInt(const std::string& name, int value) const;
        // ------------------------------------------------------------------------
        void setFloat(const std::string& name, float value) const;

      private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}  // namespace nex
