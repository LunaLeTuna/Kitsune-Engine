#pragma once

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class model{
    public:
    string name;
    string Location;
    unsigned int VBO, VAO;
    int rcount;
    vector<Vertex> vertices;

    void cube() {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void load_obj(string code){


        vector<glm::fvec3> verts;
        vector<glm::fvec2> texcords;
        vector<glm::fvec3> normals;

        vector<GLint> vf;
        vector<GLint> tf;
        vector<GLint> nf;

        string mode = "owo";
        int load = 1;

        glm::vec3 temp_vec3;
        glm::vec2 temp_vec2;
        GLint temp_glint = 0;

        stringstream ss;
        stringstream src(code);
        string line;
        string prefix;
        while(getline(src,line)){

            ss.clear();
            ss.str(line);
            ss >> prefix;

            if(prefix == "#"){

            }else if(prefix == "o"){

            }else if(prefix == "s"){

            }else if(prefix == "use_mtl"){

            }else if(prefix == "v"){ //vertexs
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                verts.push_back(temp_vec3);
            }else if(prefix == "vt"){ //
                ss >> temp_vec2.x >> temp_vec2.y;
                texcords.push_back(temp_vec2);
            }else if(prefix == "vn"){
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                normals.push_back(temp_vec3);
            }else if(prefix == "f"){
                int counter = 0;
                while(ss >> temp_glint){
                    if(counter == 0)
                        vf.push_back(temp_glint);
                    else if(counter == 1)
                        tf.push_back(temp_glint);
                    else if(counter == 2)
                        nf.push_back(temp_glint);

                    if(ss.peek() == '/'){
                        ++counter;
                        ss.ignore(1,'/');
                    }else if(ss.peek() == ' '){
                        ++counter;
                        ss.ignore(1, ' ');
                    }

                    //Reset the counter
                    if(counter > 2)
                        counter = 0;
                }
            }else{

            }

            //cout << line << endl;
            //cout << "Nr of vertices: " << vf.size() << endl;
        }

        vertices.resize(vf.size(), Vertex());
        rcount = vertices.size();

        for(size_t i = 0; i < rcount; ++i) {
            vertices[i].position = verts[vf[i]-1];
            vertices[i].texcoord = texcords[tf[i]-1];
            vertices[i].normal = normals[nf[i]-1];
            //cout << i << "  x: " << vertices[i].position.x << "  y: " << vertices[i].position.y <<"  z: " << vertices[i].position.z << endl;
            //cout << i << "  tx: " << vertices[i].texcoord.x << "  ty: " << vertices[i].texcoord.y << endl;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        //cout << "  vau: " << VAO << endl;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // texture coord attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    }

    void del() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};