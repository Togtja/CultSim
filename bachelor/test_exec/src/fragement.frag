#version 450 core
out vec4 color;
layout(location=0) in vec3 vs_col;
void main(){
    color = vec4(vs_col, 1.f);
}
