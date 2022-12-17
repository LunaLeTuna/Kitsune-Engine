#pragma once

/*
    shader_attribute types
    0 = int
    1 = float
    2 = vector2
    3 = vector3
    4 = vector4
    5 = bool
*/

class shader_attribute{
    public:
    string name;
    void* data;
    uint8_t type;
};

class shader{
    public:
    int success;

    vector<shader_attribute> attributes;
 
    string name;
    string Location;
    string Vlast_changed;
    string Flast_changed;

    unsigned int program; //id or what

void Act_Hotswap(string location, string vt, string ft){
        unsigned int vertex;
        unsigned int fragment;
        Location = location;
        Flast_changed = ft;
        Vlast_changed = vt;

        char infoLog[621];

        // shader_info_handler(location);

        string va = get_file(location+".vert");

        const char *vav = va.c_str();

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vav, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        string ay = get_file(location+".frag");

        const char *aya = ay.c_str();

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &aya, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

            unsigned int last_program = program;
            unsigned int new_program = glCreateProgram();
            glAttachShader(new_program, vertex);
            glAttachShader(new_program, fragment);
            glLinkProgram(new_program);

            glGetProgramiv(new_program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(new_program, 621, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            cout << new_program << " " << last_program << endl;
            program = new_program;
            glDeleteProgram(last_program);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
    }

    void craft(string location){
            unsigned int vertex;
    unsigned int fragment;
        Location = location;
        Flast_changed = get_file_date(Location+".frag");
        Vlast_changed = get_file_date(Location+".vert");

        char infoLog[621];

        // shader_info_handler(location);

        string va = get_file(location+".vert");

        const char *vav = va.c_str();

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vav, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        string ay = get_file(location+".frag");

        const char *aya = ay.c_str();

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &aya, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 621, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

            program = glCreateProgram();
            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            glLinkProgram(program);

            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(program, 621, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            glDeleteShader(vertex);
            glDeleteShader(fragment);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
    }
    void setVec3(const char* name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program, name), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void load_attribute(){
        for (auto& attribute : attributes){
            switch(attribute.type) {
                case 0:
                {
                    int* back = static_cast<int*>(attribute.data);
                    setInt(attribute.name, *back);
                    break;
                }
                case 1:
                {
                    float* back = static_cast<float*>(attribute.data);
                    setFloat(attribute.name, *back);
                    break;
                }
                case 2:
                {
                    glm::vec2* back = static_cast<glm::vec2*>(attribute.data);
                    setVec2(attribute.name, back->x, back->y);
                    break;
                }
                case 3:
                {
                    glm::vec3* back = static_cast<glm::vec3*>(attribute.data);
                    setVec3(attribute.name, back->x, back->y, back->z);
                    break;
                }
                case 5:
                {
                    bool* back = static_cast<bool*>(attribute.data);
                    setBool(attribute.name, *back);
                    break;
                }
                default:
                    cout << "unknown attribute type in " << name << endl;
            }
        }
    }

    void audit_attribute(string name, void* data, uint8_t type){
        for (auto& attribute : attributes){
            if(attribute.name.compare(name) == 0){
                attribute.data = data;
                attribute.type = type;
                return;
            }
        }
        shader_attribute temp;
        temp.name = name;
        temp.data = data;
        temp.type = type;
        attributes.push_back(temp);
    }

};