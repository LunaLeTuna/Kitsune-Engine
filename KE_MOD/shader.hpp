#pragma once

class shader{
    public:
    int success;
 
    string name;
    unsigned int vertex;
    unsigned int fragment;
    unsigned int program; //id or what

    // uint8_t settings;
    // // 00000000
    // //first bit is give time
    // //second bit is give lights

    // void shader_info_handler(string location){
    //     //kitsune engine shader info B3
    //     string code = get_file(location+".kesi");

    //     uint8_t tempside = 0;
    //     string loadtemp = "";

    //     uint8_t temp_setting = 0;

    //     stringstream ss;
    //     stringstream src(code);
    //     string line;
    //     string prefix;
    //     while(getline(src,line)){
    //         ss.clear();
    //         ss.str(line);
    //         ss >> prefix;

    //         if(prefix == "#"){

    //         }else if(prefix == "give_time"){
    //             ss >> temp_setting;
    //             if(temp_setting) settings += 1;
    //         }else if(prefix == "load_lights"){
    //             ss >> temp_setting;
    //             if(temp_setting) settings += 2;
    //         }else if(prefix == "load_screen"){
    //             ss >> temp_setting;
    //             if(temp_setting) settings += 4;
    //         }else{
                
    //         }
    //     }
    //     cout << "nya nya mf: " << settings << endl;
    // }

    void craft(string location){

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

};