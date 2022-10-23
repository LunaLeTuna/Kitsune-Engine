shader image_base_shader;

class image_element{
public:
    shader* shaderz;

    glm::vec2 position;
    glm::vec2 scale;
    
    bool flipx = false;
    bool flipy = false;

    texture* imbase;

    texture* imbase2;
    bool has2 = false;

    GLuint text0, text1;
    bool textset = false;

    unsigned int VAO, VBO;

    bool loaded = 0;

    void Render(){

        if(!textset){
            glGenTextures(1, &text0);
            glBindTexture(GL_TEXTURE_2D, text0);
            glGenTextures(1, &text1);
            glBindTexture(GL_TEXTURE_2D, text1); 
        }

        glUseProgram(shaderz->program);
        glUniform1i(text0, 0);
        glUniform1i(text1,  1);
        
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
        glUniformMatrix4fv(glGetUniformLocation(shaderz->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        shaderz->setBool("FlipX",flipx);
        shaderz->setBool("FlipY",flipy);
        
        glBindVertexArray(VAO);

        float w = scale.x;
        float h = scale.y;

        float xpos = position.x-(scale.x/2);
        float ypos = position.y-(scale.y/2);

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
        shaderz->setInt("text0", 0);
        imbase->call();

        if(has2){
            glActiveTexture(GL_TEXTURE1);
            shaderz->setInt("text1", 1);
            imbase2->call();
        }
        
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