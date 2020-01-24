#version 450 core
layout(location=0) in vec2 aPosition;
layout(location=0) uniform float uTime = 0.f;
layout(location=1) uniform vec3 uColor = vec3(0.3f, 0.8f, 0.6f);
layout(location=0) out vec3 vs_col;
void main() {
      mat2 rotMatrix = mat2(cos(uTime), -sin(uTime), sin(uTime), cos(uTime));
      gl_Position = vec4(rotMatrix * aPosition, 0.f, 1.f);
      vs_col = uColor;
}
