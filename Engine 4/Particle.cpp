#include "Particle.h"


// =========================== ParticleSystem ===============

std::vector<GLfloat> ParticleSystem::g_vertex_buffer_data = {
 -0.5f, -0.5f, 0.0f,
 0.5f, -0.5f, 0.0f,
 -0.5f, 0.5f, 0.0f,
 0.5f, 0.5f, 0.0f,
};

float ParticleSystem::aspectRatio = 1.0;

void ParticleSystem::setAspectRatio(float aspectRatio)
{
    float y = 0.5;
    float x = 0.5 / aspectRatio;// *aspectRatio;

    ParticleSystem::aspectRatio = aspectRatio;

    ParticleSystem::g_vertex_buffer_data = {
         -x, -y, 0.0f,
         x, -y, 0.0f,
         -x, y, 0.0f,
         x, y, 0.0f,
    };

    glBindVertexArray(particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, particleVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

}

ParticleSystem::ParticleSystem()
{
    this->persistentVisible = true;

    //http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

    // The fullscreen quad's FBO
    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);

    glGenBuffers(1, &particleVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, particleVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particlePositionBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, particlePositionBufferID);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particleColorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, particleColorBufferID);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    GLenum err;
    while ((err = glGetError())) {
        std::cout << "stuff" << " : error : " << (err) << "\n";
    }

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, particleVertexBufferID);
    glVertexAttribPointer(
        0, // attribute. No particular reason for 0, but must match the layout in the shader.
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0, // stride
        (void*)0 // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particlePositionBufferID);
    glVertexAttribPointer(
        1, // attribute. No particular reason for 1, but must match the layout in the shader.
        4, // size : x + y + z + size => 4
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0, // stride
        (void*)0 // array buffer offset
    );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particleColorBufferID);
    glVertexAttribPointer(
        2, // attribute. No particular reason for 1, but must match the layout in the shader.
        4, // size : r + g + b + a => 4
        GL_FLOAT, // type
        GL_FALSE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
        0, // stride
        (void*)0 // array buffer offset
    );

    glBindVertexArray(0);

    while ((err = glGetError())) {
        std::cout << "binds" << " : error : " << (err) << "\n";
    }

    colorMatrix = glm::mat4(1.0);
}

void ParticleSystem::updateParticles(float dt)
{
    int indx = 0;

    for (int i = 0; i < particles.size(); i++)
    {
        particles[i].life -= dt;

        if (particles[i].life > 0.0)
        {
            particles[i].vel += dt * particles[i].acc;
            particles[i].pos += dt * particles[i].vel;
            particles[indx] = particles[i];
            indx++;
        }
    }

    particles.resize(indx);

    particlePos.resize(4 * particles.size());
    particleCol.resize(4 * particles.size());


    for (int i = 0; i < particles.size(); i++)
    {
        particlePos[4 * i + 0] = particles[i].pos.x;
        particlePos[4 * i + 1] = particles[i].pos.y;
        particlePos[4 * i + 2] = particles[i].pos.z;
        particlePos[4 * i + 3] = particles[i].size;
                        
        particleCol[4 * i + 0] = particles[i].col.r;
        particleCol[4 * i + 1] = particles[i].col.g;
        particleCol[4 * i + 2] = particles[i].col.b;
        particleCol[4 * i + 3] = 1.0;
    }

    glBindVertexArray(particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, particlePositionBufferID);
    glBufferData(GL_ARRAY_BUFFER, particlePos.size()* sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlePos.size() * sizeof(GLfloat), &particlePos[0]);

    glBindBuffer(GL_ARRAY_BUFFER, particleColorBufferID);
    glBufferData(GL_ARRAY_BUFFER, particleCol.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCol.size() * sizeof(GLfloat), &particleCol[0]);

    glBindVertexArray(0);
}

void ParticleSystem::spawnParticle(glm::vec3 pos, float size, glm::vec3 color, glm::vec3 vel, glm::vec3 acc, float life)
{
    Particle part(pos, size, vel, acc, color, life);
    particles.push_back(part);
}

void ParticleSystem::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
    glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
    glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
    glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
    glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
    glm::mat4 NMmatrix = glm::transpose(glm::inverse(MVMatrix));

    if (shader)
    {
        shader->useShader();
        shader->setMatrixes(MVPmatrix, MVMatrix, NMmatrix, MMatrix, colorMatrix);

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glBindVertexArray(particleVAO);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad -> 1

        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        //std::cout << "parts\n";
        glBindVertexArray(0);
    }

}
