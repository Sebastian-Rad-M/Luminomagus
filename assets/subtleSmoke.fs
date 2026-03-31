#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

// Raylib automatically passes the texture you are currently drawing into texture0
uniform sampler2D texture0; 
uniform float u_time;

// A pseudo-random hash function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Basic 2D Value Noise
float noise(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Fractional Brownian Motion
float fbm(in vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    vec2 shift = vec2(100.0);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < 5; ++i) {
        value += amplitude * noise(st);
        st = rot * st * 2.0 + shift;
        amplitude *= 0.5;
    }
    return value;
}
void main() {
    vec4 bgPixel = texture(texture0, fragTexCoord);
    vec2 st = fragTexCoord * 5.0; 

    vec2 q = vec2(0.0);
    q.x = fbm(st + 0.04 * u_time);
    q.y = fbm(st + vec2(1.0));

    vec2 r = vec2(0.0);
    r.x = fbm(st + 1.0 * q + vec2(1.7, 9.2) + 0.03 * u_time);
    r.y = fbm(st + 1.0 * q + vec2(8.3, 2.8) + 0.05 * u_time);

    float f = fbm(st + r);

    vec3 smokeColor = vec3(0.220, 0.216, 0.216); 
    
    float smokeIntensity = f * f * 2.0; 
    
float verticalMask = smoothstep(-0.3, 0.8, fragTexCoord.y);    
    float masterOpacity = 0.5;
    float finalSmokeAlpha = clamp(smokeIntensity * verticalMask * masterOpacity, 0.0, 1.0);

    vec3 blendedColor = mix(bgPixel.rgb, smokeColor, finalSmokeAlpha);
    finalColor = vec4(blendedColor, bgPixel.a) * fragColor; 
}