shader image_base_shader;

class image_element{
public:
    shader* shaderz;

    glm::vec2 position;
    glm::vec2 scale;

    texture* imbase;

    unsigned int VAO, VBO;

    bool loaded = 0;

    void Render(){

        glUseProgram(shaderz->program);
        
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
        glUniformMatrix4fv(glGetUniformLocation(shaderz->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);

        float xpos = position.x;
        float ypos = position.y;

        float w = scale.x;
        float h = scale.y;

        float vertices[] = {
            xpos,     ypos + h,   0.0f, 0.0f ,
            xpos,     ypos,       0.0f, 1.0f ,
            xpos + w, ypos,       1.0f, 1.0f ,

            xpos,     ypos + h,   0.0f, 0.0f ,
            xpos + w, ypos,       1.0f, 1.0f ,
            xpos + w, ypos + h,   1.0f, 0.0f 
        };
        // render glyph texture over quad
        glActiveTexture(GL_TEXTURE0);
        imbase->call();
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};