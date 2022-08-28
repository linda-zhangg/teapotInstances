#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uColor;

//phong shading data
uniform vec3 ulightcolor;
uniform vec3 uspeccolor;
uniform float ushininess;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
    vec3 instanceColors;
} f_in;

uniform sampler2D u_texture;

//bools to determine the type of colours
uniform bool useColorInstances;
uniform bool loadTexture;


// framebuffer output
out vec4 fb_color;

void main() {
    
    //ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * ulightcolor;
    
    //diffuse
    vec3 norm = normalize(f_in.normal);
    vec3 lightDirection = normalize(-f_in.position);
    float diff = max(dot(norm, lightDirection),0.0);
    vec3 diffuse = diff * ulightcolor;
    
    //specular
    float specularStrength = 0.5;
    vec3 reflectDirection = reflect(-lightDirection, norm);
    vec3 viewDirection = lightDirection;
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), ushininess);
    vec3 specular = specularStrength * spec * uspeccolor;
    
    //enable different functionalities to be toggled on and off
    vec3 result;
    if(!useColorInstances) result = (ambient + diffuse + specular) * uColor;
    else result = (ambient + diffuse + specular) * f_in.instanceColors;
    
    if(loadTexture) result *= vec3(texture(u_texture, f_in.textureCoord));

	// output to the frambuffer
	fb_color = vec4(result, 1);
}
