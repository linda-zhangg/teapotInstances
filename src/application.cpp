
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "application.hpp"
#include "bounding_box.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"
#include "cgra/cgra_mesh.hpp"

//to print vecs and mats (for testing)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


using namespace std;
using namespace cgra;
using namespace glm;
const float tau = glm::two_pi<float>();


Application::Application(GLFWwindow *window) : m_window(window) {
    m_model.mesh.transformations.clear();
    m_model.mesh.instanceColors.clear();
    boundingBox_mesh.clear();
	
	// build the shader for the model
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_vert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	GLuint color_shader = color_sb.build();

	// build the mesh for the model
	mesh_builder teapot_mb = load_wavefront_data(CGRA_SRCDIR + std::string("//res//assets//teapot.obj"));
	gl_mesh teapot_mesh = teapot_mb.build();

	// put together an object
	m_model.shader = color_shader;
	m_model.mesh = teapot_mesh;
	m_model.color = glm::vec3(0.8, 1, 1);
	m_model.modelTransform = glm::mat4(1);
    //phong shading model variables
    m_model.lightcolor = glm::vec3(0.8, 0.5, 1);
    m_model.speccolor = glm::vec3(1, 1, 1);
    m_model.shininess = 20;
    
    //bounding boxes
    for(unsigned int i=0; i<m_model.mesh.transformations.size(); i++){
        calculateBoundingBox(teapot_mb, i);
    }
}

void Application::calculateBoundingBox(mesh_builder teapot, int transIndex){
    //transform each vertex position by the instance transformation
    for(unsigned int i = 0; i < teapot.vertices.size(); i++){
        teapot.vertices[i].pos = m_model.mesh.transformations.at(transIndex) * vec4(teapot.vertices[i].pos,1);
    }
    
    //calculate boundaries
    GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
    min_x = max_x = teapot.vertices[0].pos.x;
    min_y = max_y = teapot.vertices[0].pos.y;
    min_z = max_z = teapot.vertices[0].pos.z;
    for(unsigned int i = 0; i < teapot.vertices.size(); i++){
        if (teapot.vertices[i].pos.x < min_x) min_x = teapot.vertices[i].pos.x;
        if (teapot.vertices[i].pos.x > max_x) max_x = teapot.vertices[i].pos.x;
        if (teapot.vertices[i].pos.y < min_y) min_y = teapot.vertices[i].pos.y;
        if (teapot.vertices[i].pos.y > max_y) max_y = teapot.vertices[i].pos.y;
        if (teapot.vertices[i].pos.z < min_z) min_z = teapot.vertices[i].pos.z;
        if (teapot.vertices[i].pos.z > max_z) max_z = teapot.vertices[i].pos.z;
    }
    minV = vec3(min_x, min_y, min_z);
    maxV = vec3(max_x, max_y, max_z);
    //add to list of bounding box meshes
    boundingBox_mesh.push_back(createBoundingBoxMesh(minV, maxV));
}


void Application::render() {
	
	// retrieve the window hieght
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);
    
    mat4 view = mat4(1.0f); //identity matrix
    mat4 trans = translate(view,vec3(0, 0, -m_distance));
    mat4 rotateY = rotate(view, m_pitch, vec3(1.0f, 0.0f, 0.0f));
    mat4 rotateX = rotate(view, m_yaw, vec3(0.0f, 1.0f, 0.0f));
    view = trans * rotateY * rotateX * view;
    
	// draw options
	if (m_show_grid) cgra::drawGrid(view, proj);
	if (m_show_axis) cgra::drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

	// draw the model
	m_model.draw(view, proj);
    
    //bounding box
    if(m_showBoundingBox) {
        if(!m_model.mesh.drawInstances) boundingBox_mesh.at(0).draw();
        else{
            for(unsigned int i=0; i< boundingBox_mesh.size(); i++){ //for all the instances
                boundingBox_mesh.at(i).draw();
            }
        }
    }
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(360, 200), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //pitch and yaw
    ImGui::SliderFloat("Pitch", &m_pitch, -tau, tau);
    ImGui::SliderFloat("Yaw", &m_yaw, -tau, tau);
    ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.1f");
	ImGui::SliderFloat3("Model Color", value_ptr(m_model.color), 0, 1, "%.2f");
    //Phong shading
    ImGui::SliderFloat3("Light Color", value_ptr(m_model.lightcolor), 0, 1, "%.2f");
    ImGui::SliderFloat3("Specular Color", value_ptr(m_model.speccolor), 0, 1, "%.2f");
    ImGui::SliderFloat("Shininess", &m_model.shininess, 1.0, 100, "%.1f");
    
    //more instances - toggle on and off drawing instances
    if(ImGui::Button("More instances")){
        m_model.mesh.drawInstances = ! m_model.mesh.drawInstances;
    }
    
    //use different colour for each instacne - toggle on and off
    if(ImGui::Button("Use colour instances")){
        m_model.useColorInstances = !m_model.useColorInstances;
    }
    
    //use textures when drawing colour - toggle on and off
    if(ImGui::Button("Load texture")){
        m_model.loadTexture = !m_model.loadTexture;
    }
    
    //draw bounding boxes - toggle on and off
    if(ImGui::Button("Draw bounding box")){
        m_showBoundingBox = !m_showBoundingBox;
    }
    
	// extra drawing parameters
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
    double xmid = 500; double ymid = 350; //centre point in world
    if(ImGui::IsMouseDragging()){
        double xdiff = xpos - xmid;
        m_yaw = xdiff * (tau/xmid);
        double ydiff = ypos - ymid;
        m_pitch = ydiff * (tau/ymid);
    }
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void)button, action, mods; // currently un-used
}


void Application::scrollCallback(double xoffset, double yoffset) {
    //working with inversely proportional scroll wheel
    //increase distance = negative scroll, decrease distance = positive scroll
    m_distance = yoffset < 0 ? m_distance + 1 : m_distance - 1;
    if(m_distance > 200) m_distance = 200;
    if(m_distance < 0) m_distance = 0;
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}
