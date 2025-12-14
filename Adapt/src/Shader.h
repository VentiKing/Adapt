#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    // Shader program ID
    unsigned int ID;

    // Constructor that reads and builds the shader from files
    Shader(const char* vertexPath, const char* fragmentPath);

    // Method to activate the shader program
    void use();

    // Utility methods to set uniform variables
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    // Method to set a mat4 uniform variable
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};