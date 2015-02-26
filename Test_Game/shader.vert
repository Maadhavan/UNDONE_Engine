#version 140 

uniform mat4 gMVP;

in vec3 inPosition; 
in vec3 inNormal;
 
smooth out vec3 theColor; 

void main() 
{ 
   gl_Position = gMVP * vec4(inPosition, 1.0);
   vec3 transNormal = (gMVP* vec4(inNormal,0.0f)).xyz;
   normalize(transNormal); 
   vec3 lightdir = vec3(0.0f,-0.5f,-0.5f);
   vec3 light2dir = vec3(-0.5,-0.5,0.0f);
   vec3 Ambient = vec3(0.3,0.3,0.3);
   float mag = dot(lightdir,transNormal);
   theColor = (vec3(1.0f,1.0f,1.0f)*(mag))+Ambient ;
}